// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/instruction-selector-impl.h"
#include "src/compiler/node-matchers.h"
#include "src/compiler/node-properties-inl.h"

namespace v8 {
namespace internal {
namespace compiler {

// Adds IA32-specific methods for generating operands.
class IA32OperandGenerator FINAL : public OperandGenerator {
 public:
  explicit IA32OperandGenerator(InstructionSelector* selector)
      : OperandGenerator(selector) {}

  InstructionOperand* UseByteRegister(Node* node) {
    // TODO(dcarney): relax constraint.
    return UseFixed(node, edx);
  }

  bool CanBeImmediate(Node* node) {
    switch (node->opcode()) {
      case IrOpcode::kInt32Constant:
      case IrOpcode::kNumberConstant:
      case IrOpcode::kExternalConstant:
        return true;
      case IrOpcode::kHeapConstant: {
        // Constants in new space cannot be used as immediates in V8 because
        // the GC does not scan code objects when collecting the new generation.
        Unique<HeapObject> value = OpParameter<Unique<HeapObject> >(node);
        return !isolate()->heap()->InNewSpace(*value.handle());
      }
      default:
        return false;
    }
  }

  bool CanBeBetterLeftOperand(Node* node) const {
    return !selector()->IsLive(node);
  }
};


// Get the AddressingMode of scale factor N from the AddressingMode of scale
// factor 1.
static AddressingMode AdjustAddressingMode(AddressingMode base_mode,
                                           int power) {
  DCHECK(0 <= power && power < 4);
  return static_cast<AddressingMode>(static_cast<int>(base_mode) + power);
}


class AddressingModeMatcher {
 public:
  AddressingModeMatcher(IA32OperandGenerator* g, Node* base, Node* index)
      : base_operand_(NULL),
        index_operand_(NULL),
        displacement_operand_(NULL),
        mode_(kMode_None) {
    Int32Matcher index_imm(index);
    if (index_imm.HasValue()) {
      int32_t displacement = index_imm.Value();
      // Compute base operand and fold base immediate into displacement.
      Int32Matcher base_imm(base);
      if (!base_imm.HasValue()) {
        base_operand_ = g->UseRegister(base);
      } else {
        displacement += base_imm.Value();
      }
      if (displacement != 0 || base_operand_ == NULL) {
        displacement_operand_ = g->TempImmediate(displacement);
      }
      if (base_operand_ == NULL) {
        mode_ = kMode_MI;
      } else {
        if (displacement == 0) {
          mode_ = kMode_MR;
        } else {
          mode_ = kMode_MRI;
        }
      }
    } else {
      // Compute index and displacement.
      IndexAndDisplacementMatcher matcher(index);
      index_operand_ = g->UseRegister(matcher.index_node());
      int32_t displacement = matcher.displacement();
      // Compute base operand and fold base immediate into displacement.
      Int32Matcher base_imm(base);
      if (!base_imm.HasValue()) {
        base_operand_ = g->UseRegister(base);
      } else {
        displacement += base_imm.Value();
      }
      // Compute displacement operand.
      if (displacement != 0) {
        displacement_operand_ = g->TempImmediate(displacement);
      }
      // Compute mode with scale factor one.
      if (base_operand_ == NULL) {
        if (displacement_operand_ == NULL) {
          mode_ = kMode_M1;
        } else {
          mode_ = kMode_M1I;
        }
      } else {
        if (displacement_operand_ == NULL) {
          mode_ = kMode_MR1;
        } else {
          mode_ = kMode_MR1I;
        }
      }
      // Adjust mode to actual scale factor.
      mode_ = AdjustAddressingMode(mode_, matcher.power());
    }
    DCHECK_NE(kMode_None, mode_);
  }

  size_t SetInputs(InstructionOperand** inputs) {
    size_t input_count = 0;
    // Compute inputs_ and input_count.
    if (base_operand_ != NULL) {
      inputs[input_count++] = base_operand_;
    }
    if (index_operand_ != NULL) {
      inputs[input_count++] = index_operand_;
    }
    if (displacement_operand_ != NULL) {
      inputs[input_count++] = displacement_operand_;
    }
    DCHECK_NE(input_count, 0);
    return input_count;
  }

  static const int kMaxInputCount = 3;
  InstructionOperand* base_operand_;
  InstructionOperand* index_operand_;
  InstructionOperand* displacement_operand_;
  AddressingMode mode_;
};


void InstructionSelector::VisitLoad(Node* node) {
  MachineType rep = RepresentationOf(OpParameter<LoadRepresentation>(node));
  MachineType typ = TypeOf(OpParameter<LoadRepresentation>(node));
  Node* base = node->InputAt(0);
  Node* index = node->InputAt(1);

  ArchOpcode opcode;
  // TODO(titzer): signed/unsigned small loads
  switch (rep) {
    case kRepFloat32:
      opcode = kIA32Movss;
      break;
    case kRepFloat64:
      opcode = kIA32Movsd;
      break;
    case kRepBit:  // Fall through.
    case kRepWord8:
      opcode = typ == kTypeInt32 ? kIA32Movsxbl : kIA32Movzxbl;
      break;
    case kRepWord16:
      opcode = typ == kTypeInt32 ? kIA32Movsxwl : kIA32Movzxwl;
      break;
    case kRepTagged:  // Fall through.
    case kRepWord32:
      opcode = kIA32Movl;
      break;
    default:
      UNREACHABLE();
      return;
  }

  IA32OperandGenerator g(this);
  AddressingModeMatcher matcher(&g, base, index);
  InstructionCode code = opcode | AddressingModeField::encode(matcher.mode_);
  InstructionOperand* outputs[] = {g.DefineAsRegister(node)};
  InstructionOperand* inputs[AddressingModeMatcher::kMaxInputCount];
  size_t input_count = matcher.SetInputs(inputs);
  Emit(code, 1, outputs, input_count, inputs);
}


void InstructionSelector::VisitStore(Node* node) {
  IA32OperandGenerator g(this);
  Node* base = node->InputAt(0);
  Node* index = node->InputAt(1);
  Node* value = node->InputAt(2);

  StoreRepresentation store_rep = OpParameter<StoreRepresentation>(node);
  MachineType rep = RepresentationOf(store_rep.machine_type());
  if (store_rep.write_barrier_kind() == kFullWriteBarrier) {
    DCHECK_EQ(kRepTagged, rep);
    // TODO(dcarney): refactor RecordWrite function to take temp registers
    //                and pass them here instead of using fixed regs
    // TODO(dcarney): handle immediate indices.
    InstructionOperand* temps[] = {g.TempRegister(ecx), g.TempRegister(edx)};
    Emit(kIA32StoreWriteBarrier, NULL, g.UseFixed(base, ebx),
         g.UseFixed(index, ecx), g.UseFixed(value, edx), arraysize(temps),
         temps);
    return;
  }
  DCHECK_EQ(kNoWriteBarrier, store_rep.write_barrier_kind());

  ArchOpcode opcode;
  switch (rep) {
    case kRepFloat32:
      opcode = kIA32Movss;
      break;
    case kRepFloat64:
      opcode = kIA32Movsd;
      break;
    case kRepBit:  // Fall through.
    case kRepWord8:
      opcode = kIA32Movb;
      break;
    case kRepWord16:
      opcode = kIA32Movw;
      break;
    case kRepTagged:  // Fall through.
    case kRepWord32:
      opcode = kIA32Movl;
      break;
    default:
      UNREACHABLE();
      return;
  }

  InstructionOperand* val;
  if (g.CanBeImmediate(value)) {
    val = g.UseImmediate(value);
  } else if (rep == kRepWord8 || rep == kRepBit) {
    val = g.UseByteRegister(value);
  } else {
    val = g.UseRegister(value);
  }

  AddressingModeMatcher matcher(&g, base, index);
  InstructionCode code = opcode | AddressingModeField::encode(matcher.mode_);
  InstructionOperand* inputs[AddressingModeMatcher::kMaxInputCount + 1];
  size_t input_count = matcher.SetInputs(inputs);
  inputs[input_count++] = val;
  Emit(code, 0, static_cast<InstructionOperand**>(NULL), input_count, inputs);
}


// Shared routine for multiple binary operations.
static void VisitBinop(InstructionSelector* selector, Node* node,
                       InstructionCode opcode, FlagsContinuation* cont) {
  IA32OperandGenerator g(selector);
  Int32BinopMatcher m(node);
  Node* left = m.left().node();
  Node* right = m.right().node();
  InstructionOperand* inputs[4];
  size_t input_count = 0;
  InstructionOperand* outputs[2];
  size_t output_count = 0;

  // TODO(turbofan): match complex addressing modes.
  if (left == right) {
    // If both inputs refer to the same operand, enforce allocating a register
    // for both of them to ensure that we don't end up generating code like
    // this:
    //
    //   mov eax, [ebp-0x10]
    //   add eax, [ebp-0x10]
    //   jo label
    InstructionOperand* const input = g.UseRegister(left);
    inputs[input_count++] = input;
    inputs[input_count++] = input;
  } else if (g.CanBeImmediate(right)) {
    inputs[input_count++] = g.UseRegister(left);
    inputs[input_count++] = g.UseImmediate(right);
  } else {
    if (node->op()->HasProperty(Operator::kCommutative) &&
        g.CanBeBetterLeftOperand(right)) {
      std::swap(left, right);
    }
    inputs[input_count++] = g.UseRegister(left);
    inputs[input_count++] = g.Use(right);
  }

  if (cont->IsBranch()) {
    inputs[input_count++] = g.Label(cont->true_block());
    inputs[input_count++] = g.Label(cont->false_block());
  }

  outputs[output_count++] = g.DefineSameAsFirst(node);
  if (cont->IsSet()) {
    // TODO(turbofan): Use byte register here.
    outputs[output_count++] = g.DefineAsRegister(cont->result());
  }

  DCHECK_NE(0, input_count);
  DCHECK_NE(0, output_count);
  DCHECK_GE(arraysize(inputs), input_count);
  DCHECK_GE(arraysize(outputs), output_count);

  Instruction* instr = selector->Emit(cont->Encode(opcode), output_count,
                                      outputs, input_count, inputs);
  if (cont->IsBranch()) instr->MarkAsControl();
}


// Shared routine for multiple binary operations.
static void VisitBinop(InstructionSelector* selector, Node* node,
                       InstructionCode opcode) {
  FlagsContinuation cont;
  VisitBinop(selector, node, opcode, &cont);
}


void InstructionSelector::VisitWord32And(Node* node) {
  VisitBinop(this, node, kIA32And);
}


void InstructionSelector::VisitWord32Or(Node* node) {
  VisitBinop(this, node, kIA32Or);
}


void InstructionSelector::VisitWord32Xor(Node* node) {
  IA32OperandGenerator g(this);
  Int32BinopMatcher m(node);
  if (m.right().Is(-1)) {
    Emit(kIA32Not, g.DefineSameAsFirst(node), g.UseRegister(m.left().node()));
  } else {
    VisitBinop(this, node, kIA32Xor);
  }
}


// Shared routine for multiple shift operations.
static inline void VisitShift(InstructionSelector* selector, Node* node,
                              ArchOpcode opcode) {
  IA32OperandGenerator g(selector);
  Node* left = node->InputAt(0);
  Node* right = node->InputAt(1);

  if (g.CanBeImmediate(right)) {
    selector->Emit(opcode, g.DefineSameAsFirst(node), g.UseRegister(left),
                   g.UseImmediate(right));
  } else {
    Int32BinopMatcher m(node);
    if (m.right().IsWord32And()) {
      Int32BinopMatcher mright(right);
      if (mright.right().Is(0x1F)) {
        right = mright.left().node();
      }
    }
    selector->Emit(opcode, g.DefineSameAsFirst(node), g.UseRegister(left),
                   g.UseFixed(right, ecx));
  }
}


void InstructionSelector::VisitWord32Shl(Node* node) {
  VisitShift(this, node, kIA32Shl);
}


void InstructionSelector::VisitWord32Shr(Node* node) {
  VisitShift(this, node, kIA32Shr);
}


void InstructionSelector::VisitWord32Sar(Node* node) {
  VisitShift(this, node, kIA32Sar);
}


void InstructionSelector::VisitWord32Ror(Node* node) {
  VisitShift(this, node, kIA32Ror);
}


static bool TryEmitLeaMultAdd(InstructionSelector* selector, Node* node) {
  Int32BinopMatcher m(node);
  if (!m.right().HasValue()) return false;
  int32_t displacement_value = m.right().Value();
  Node* left = m.left().node();
  LeaMultiplyMatcher lmm(left);
  if (!lmm.Matches()) return false;
  AddressingMode mode;
  size_t input_count;
  IA32OperandGenerator g(selector);
  InstructionOperand* index = g.UseRegister(lmm.Left());
  InstructionOperand* displacement = g.TempImmediate(displacement_value);
  InstructionOperand* inputs[] = {index, displacement, displacement};
  if (lmm.Displacement() != 0) {
    input_count = 3;
    inputs[1] = index;
    mode = kMode_MR1I;
  } else {
    input_count = 2;
    mode = kMode_M1I;
  }
  mode = AdjustAddressingMode(mode, lmm.Power());
  InstructionOperand* outputs[] = {g.DefineAsRegister(node)};
  selector->Emit(kIA32Lea | AddressingModeField::encode(mode), 1, outputs,
                 input_count, inputs);
  return true;
}


void InstructionSelector::VisitInt32Add(Node* node) {
  if (TryEmitLeaMultAdd(this, node)) return;
  VisitBinop(this, node, kIA32Add);
}


void InstructionSelector::VisitInt32Sub(Node* node) {
  IA32OperandGenerator g(this);
  Int32BinopMatcher m(node);
  if (m.left().Is(0)) {
    Emit(kIA32Neg, g.DefineSameAsFirst(node), g.Use(m.right().node()));
  } else {
    VisitBinop(this, node, kIA32Sub);
  }
}


static bool TryEmitLeaMult(InstructionSelector* selector, Node* node) {
  LeaMultiplyMatcher lea(node);
  // Try to match lea.
  if (!lea.Matches()) return false;
  AddressingMode mode;
  size_t input_count;
  IA32OperandGenerator g(selector);
  InstructionOperand* left = g.UseRegister(lea.Left());
  InstructionOperand* inputs[] = {left, left};
  if (lea.Displacement() != 0) {
    input_count = 2;
    mode = kMode_MR1;
  } else {
    input_count = 1;
    mode = kMode_M1;
  }
  mode = AdjustAddressingMode(mode, lea.Power());
  InstructionOperand* outputs[] = {g.DefineAsRegister(node)};
  selector->Emit(kIA32Lea | AddressingModeField::encode(mode), 1, outputs,
                 input_count, inputs);
  return true;
}


void InstructionSelector::VisitInt32Mul(Node* node) {
  if (TryEmitLeaMult(this, node)) return;
  IA32OperandGenerator g(this);
  Int32BinopMatcher m(node);
  Node* left = m.left().node();
  Node* right = m.right().node();
  if (g.CanBeImmediate(right)) {
    Emit(kIA32Imul, g.DefineAsRegister(node), g.Use(left),
         g.UseImmediate(right));
  } else {
    if (g.CanBeBetterLeftOperand(right)) {
      std::swap(left, right);
    }
    Emit(kIA32Imul, g.DefineSameAsFirst(node), g.UseRegister(left),
         g.Use(right));
  }
}


void InstructionSelector::VisitInt32MulHigh(Node* node) {
  IA32OperandGenerator g(this);
  InstructionOperand* temps[] = {g.TempRegister(eax)};
  size_t temp_count = arraysize(temps);
  Emit(kIA32ImulHigh, g.DefineAsFixed(node, edx),
       g.UseFixed(node->InputAt(0), eax), g.UseRegister(node->InputAt(1)),
       temp_count, temps);
}


static inline void VisitDiv(InstructionSelector* selector, Node* node,
                            ArchOpcode opcode) {
  IA32OperandGenerator g(selector);
  InstructionOperand* temps[] = {g.TempRegister(edx)};
  size_t temp_count = arraysize(temps);
  selector->Emit(opcode, g.DefineAsFixed(node, eax),
                 g.UseFixed(node->InputAt(0), eax),
                 g.UseUnique(node->InputAt(1)), temp_count, temps);
}


void InstructionSelector::VisitInt32Div(Node* node) {
  VisitDiv(this, node, kIA32Idiv);
}


void InstructionSelector::VisitUint32Div(Node* node) {
  VisitDiv(this, node, kIA32Udiv);
}


static inline void VisitMod(InstructionSelector* selector, Node* node,
                            ArchOpcode opcode) {
  IA32OperandGenerator g(selector);
  InstructionOperand* temps[] = {g.TempRegister(eax), g.TempRegister(edx)};
  size_t temp_count = arraysize(temps);
  selector->Emit(opcode, g.DefineAsFixed(node, edx),
                 g.UseFixed(node->InputAt(0), eax),
                 g.UseUnique(node->InputAt(1)), temp_count, temps);
}


void InstructionSelector::VisitInt32Mod(Node* node) {
  VisitMod(this, node, kIA32Idiv);
}


void InstructionSelector::VisitUint32Mod(Node* node) {
  VisitMod(this, node, kIA32Udiv);
}


void InstructionSelector::VisitChangeFloat32ToFloat64(Node* node) {
  IA32OperandGenerator g(this);
  Emit(kSSECvtss2sd, g.DefineAsRegister(node), g.Use(node->InputAt(0)));
}


void InstructionSelector::VisitChangeInt32ToFloat64(Node* node) {
  IA32OperandGenerator g(this);
  Emit(kSSEInt32ToFloat64, g.DefineAsRegister(node), g.Use(node->InputAt(0)));
}


void InstructionSelector::VisitChangeUint32ToFloat64(Node* node) {
  IA32OperandGenerator g(this);
  Emit(kSSEUint32ToFloat64, g.DefineAsRegister(node), g.Use(node->InputAt(0)));
}


void InstructionSelector::VisitChangeFloat64ToInt32(Node* node) {
  IA32OperandGenerator g(this);
  Emit(kSSEFloat64ToInt32, g.DefineAsRegister(node), g.Use(node->InputAt(0)));
}


void InstructionSelector::VisitChangeFloat64ToUint32(Node* node) {
  IA32OperandGenerator g(this);
  Emit(kSSEFloat64ToUint32, g.DefineAsRegister(node), g.Use(node->InputAt(0)));
}


void InstructionSelector::VisitTruncateFloat64ToFloat32(Node* node) {
  IA32OperandGenerator g(this);
  Emit(kSSECvtsd2ss, g.DefineAsRegister(node), g.Use(node->InputAt(0)));
}


void InstructionSelector::VisitFloat64Add(Node* node) {
  IA32OperandGenerator g(this);
  Emit(kSSEFloat64Add, g.DefineSameAsFirst(node),
       g.UseRegister(node->InputAt(0)), g.UseRegister(node->InputAt(1)));
}


void InstructionSelector::VisitFloat64Sub(Node* node) {
  IA32OperandGenerator g(this);
  Emit(kSSEFloat64Sub, g.DefineSameAsFirst(node),
       g.UseRegister(node->InputAt(0)), g.UseRegister(node->InputAt(1)));
}


void InstructionSelector::VisitFloat64Mul(Node* node) {
  IA32OperandGenerator g(this);
  Emit(kSSEFloat64Mul, g.DefineSameAsFirst(node),
       g.UseRegister(node->InputAt(0)), g.UseRegister(node->InputAt(1)));
}


void InstructionSelector::VisitFloat64Div(Node* node) {
  IA32OperandGenerator g(this);
  Emit(kSSEFloat64Div, g.DefineSameAsFirst(node),
       g.UseRegister(node->InputAt(0)), g.UseRegister(node->InputAt(1)));
}


void InstructionSelector::VisitFloat64Mod(Node* node) {
  IA32OperandGenerator g(this);
  InstructionOperand* temps[] = {g.TempRegister(eax)};
  Emit(kSSEFloat64Mod, g.DefineSameAsFirst(node),
       g.UseRegister(node->InputAt(0)), g.UseRegister(node->InputAt(1)), 1,
       temps);
}


void InstructionSelector::VisitFloat64Sqrt(Node* node) {
  IA32OperandGenerator g(this);
  Emit(kSSEFloat64Sqrt, g.DefineAsRegister(node), g.Use(node->InputAt(0)));
}


void InstructionSelector::VisitCall(Node* node) {
  IA32OperandGenerator g(this);
  CallDescriptor* descriptor = OpParameter<CallDescriptor*>(node);

  FrameStateDescriptor* frame_state_descriptor = NULL;

  if (descriptor->NeedsFrameState()) {
    frame_state_descriptor =
        GetFrameStateDescriptor(node->InputAt(descriptor->InputCount()));
  }

  CallBuffer buffer(zone(), descriptor, frame_state_descriptor);

  // Compute InstructionOperands for inputs and outputs.
  InitializeCallBuffer(node, &buffer, true, true);

  // Push any stack arguments.
  for (NodeVectorRIter input = buffer.pushed_nodes.rbegin();
       input != buffer.pushed_nodes.rend(); input++) {
    // TODO(titzer): handle pushing double parameters.
    Emit(kIA32Push, NULL,
         g.CanBeImmediate(*input) ? g.UseImmediate(*input) : g.Use(*input));
  }

  // Select the appropriate opcode based on the call type.
  InstructionCode opcode;
  switch (descriptor->kind()) {
    case CallDescriptor::kCallCodeObject: {
      opcode = kArchCallCodeObject;
      break;
    }
    case CallDescriptor::kCallJSFunction:
      opcode = kArchCallJSFunction;
      break;
    default:
      UNREACHABLE();
      return;
  }
  opcode |= MiscField::encode(descriptor->flags());

  // Emit the call instruction.
  InstructionOperand** first_output =
      buffer.outputs.size() > 0 ? &buffer.outputs.front() : NULL;
  Instruction* call_instr =
      Emit(opcode, buffer.outputs.size(), first_output,
           buffer.instruction_args.size(), &buffer.instruction_args.front());
  call_instr->MarkAsCall();
}


// Shared routine for multiple compare operations.
static void VisitCompare(InstructionSelector* selector, InstructionCode opcode,
                         InstructionOperand* left, InstructionOperand* right,
                         FlagsContinuation* cont) {
  IA32OperandGenerator g(selector);
  if (cont->IsBranch()) {
    selector->Emit(cont->Encode(opcode), NULL, left, right,
                   g.Label(cont->true_block()),
                   g.Label(cont->false_block()))->MarkAsControl();
  } else {
    DCHECK(cont->IsSet());
    // TODO(titzer): Needs byte register.
    selector->Emit(cont->Encode(opcode), g.DefineAsRegister(cont->result()),
                   left, right);
  }
}


// Shared routine for multiple compare operations.
static void VisitCompare(InstructionSelector* selector, InstructionCode opcode,
                         Node* left, Node* right, FlagsContinuation* cont,
                         bool commutative) {
  IA32OperandGenerator g(selector);
  if (commutative && g.CanBeBetterLeftOperand(right)) {
    std::swap(left, right);
  }
  VisitCompare(selector, opcode, g.UseRegister(left), g.Use(right), cont);
}


// Shared routine for multiple word compare operations.
static void VisitWordCompare(InstructionSelector* selector, Node* node,
                             InstructionCode opcode, FlagsContinuation* cont) {
  IA32OperandGenerator g(selector);
  Node* const left = node->InputAt(0);
  Node* const right = node->InputAt(1);

  // Match immediates on left or right side of comparison.
  if (g.CanBeImmediate(right)) {
    VisitCompare(selector, opcode, g.Use(left), g.UseImmediate(right), cont);
  } else if (g.CanBeImmediate(left)) {
    if (!node->op()->HasProperty(Operator::kCommutative)) cont->Commute();
    VisitCompare(selector, opcode, g.Use(right), g.UseImmediate(left), cont);
  } else {
    VisitCompare(selector, opcode, left, right, cont,
                 node->op()->HasProperty(Operator::kCommutative));
  }
}


// Shared routine for multiple float compare operations.
static void VisitFloat64Compare(InstructionSelector* selector, Node* node,
                                FlagsContinuation* cont) {
  VisitCompare(selector, kSSEFloat64Cmp, node->InputAt(0), node->InputAt(1),
               cont, node->op()->HasProperty(Operator::kCommutative));
}


void InstructionSelector::VisitBranch(Node* branch, BasicBlock* tbranch,
                                      BasicBlock* fbranch) {
  IA32OperandGenerator g(this);
  Node* user = branch;
  Node* value = branch->InputAt(0);

  FlagsContinuation cont(kNotEqual, tbranch, fbranch);

  // If we can fall through to the true block, invert the branch.
  if (IsNextInAssemblyOrder(tbranch)) {
    cont.Negate();
    cont.SwapBlocks();
  }

  // Try to combine with comparisons against 0 by simply inverting the branch.
  while (CanCover(user, value) && value->opcode() == IrOpcode::kWord32Equal) {
    Int32BinopMatcher m(value);
    if (m.right().Is(0)) {
      user = value;
      value = m.left().node();
      cont.Negate();
    } else {
      break;
    }
  }

  // Try to combine the branch with a comparison.
  if (CanCover(user, value)) {
    switch (value->opcode()) {
      case IrOpcode::kWord32Equal:
        cont.OverwriteAndNegateIfEqual(kEqual);
        return VisitWordCompare(this, value, kIA32Cmp, &cont);
      case IrOpcode::kInt32LessThan:
        cont.OverwriteAndNegateIfEqual(kSignedLessThan);
        return VisitWordCompare(this, value, kIA32Cmp, &cont);
      case IrOpcode::kInt32LessThanOrEqual:
        cont.OverwriteAndNegateIfEqual(kSignedLessThanOrEqual);
        return VisitWordCompare(this, value, kIA32Cmp, &cont);
      case IrOpcode::kUint32LessThan:
        cont.OverwriteAndNegateIfEqual(kUnsignedLessThan);
        return VisitWordCompare(this, value, kIA32Cmp, &cont);
      case IrOpcode::kUint32LessThanOrEqual:
        cont.OverwriteAndNegateIfEqual(kUnsignedLessThanOrEqual);
        return VisitWordCompare(this, value, kIA32Cmp, &cont);
      case IrOpcode::kFloat64Equal:
        cont.OverwriteAndNegateIfEqual(kUnorderedEqual);
        return VisitFloat64Compare(this, value, &cont);
      case IrOpcode::kFloat64LessThan:
        cont.OverwriteAndNegateIfEqual(kUnorderedLessThan);
        return VisitFloat64Compare(this, value, &cont);
      case IrOpcode::kFloat64LessThanOrEqual:
        cont.OverwriteAndNegateIfEqual(kUnorderedLessThanOrEqual);
        return VisitFloat64Compare(this, value, &cont);
      case IrOpcode::kProjection:
        // Check if this is the overflow output projection of an
        // <Operation>WithOverflow node.
        if (OpParameter<size_t>(value) == 1u) {
          // We cannot combine the <Operation>WithOverflow with this branch
          // unless the 0th projection (the use of the actual value of the
          // <Operation> is either NULL, which means there's no use of the
          // actual value, or was already defined, which means it is scheduled
          // *AFTER* this branch).
          Node* node = value->InputAt(0);
          Node* result = node->FindProjection(0);
          if (result == NULL || IsDefined(result)) {
            switch (node->opcode()) {
              case IrOpcode::kInt32AddWithOverflow:
                cont.OverwriteAndNegateIfEqual(kOverflow);
                return VisitBinop(this, node, kIA32Add, &cont);
              case IrOpcode::kInt32SubWithOverflow:
                cont.OverwriteAndNegateIfEqual(kOverflow);
                return VisitBinop(this, node, kIA32Sub, &cont);
              default:
                break;
            }
          }
        }
        break;
      case IrOpcode::kInt32Sub:
        return VisitWordCompare(this, value, kIA32Cmp, &cont);
      case IrOpcode::kWord32And:
        return VisitWordCompare(this, value, kIA32Test, &cont);
      default:
        break;
    }
  }

  // Branch could not be combined with a compare, emit compare against 0.
  VisitCompare(this, kIA32Cmp, g.Use(value), g.TempImmediate(0), &cont);
}


void InstructionSelector::VisitWord32Equal(Node* const node) {
  Node* const user = node;
  FlagsContinuation cont(kEqual, node);
  Int32BinopMatcher m(user);
  if (m.right().Is(0)) {
    Node* const value = m.left().node();
    if (CanCover(user, value)) {
      switch (value->opcode()) {
        case IrOpcode::kInt32Sub:
          return VisitWordCompare(this, value, kIA32Cmp, &cont);
        case IrOpcode::kWord32And:
          return VisitWordCompare(this, value, kIA32Test, &cont);
        default:
          break;
      }
    }
  }
  VisitWordCompare(this, node, kIA32Cmp, &cont);
}


void InstructionSelector::VisitInt32LessThan(Node* node) {
  FlagsContinuation cont(kSignedLessThan, node);
  VisitWordCompare(this, node, kIA32Cmp, &cont);
}


void InstructionSelector::VisitInt32LessThanOrEqual(Node* node) {
  FlagsContinuation cont(kSignedLessThanOrEqual, node);
  VisitWordCompare(this, node, kIA32Cmp, &cont);
}


void InstructionSelector::VisitUint32LessThan(Node* node) {
  FlagsContinuation cont(kUnsignedLessThan, node);
  VisitWordCompare(this, node, kIA32Cmp, &cont);
}


void InstructionSelector::VisitUint32LessThanOrEqual(Node* node) {
  FlagsContinuation cont(kUnsignedLessThanOrEqual, node);
  VisitWordCompare(this, node, kIA32Cmp, &cont);
}


void InstructionSelector::VisitInt32AddWithOverflow(Node* node) {
  if (Node* ovf = node->FindProjection(1)) {
    FlagsContinuation cont(kOverflow, ovf);
    return VisitBinop(this, node, kIA32Add, &cont);
  }
  FlagsContinuation cont;
  VisitBinop(this, node, kIA32Add, &cont);
}


void InstructionSelector::VisitInt32SubWithOverflow(Node* node) {
  if (Node* ovf = node->FindProjection(1)) {
    FlagsContinuation cont(kOverflow, ovf);
    return VisitBinop(this, node, kIA32Sub, &cont);
  }
  FlagsContinuation cont;
  VisitBinop(this, node, kIA32Sub, &cont);
}


void InstructionSelector::VisitFloat64Equal(Node* node) {
  FlagsContinuation cont(kUnorderedEqual, node);
  VisitFloat64Compare(this, node, &cont);
}


void InstructionSelector::VisitFloat64LessThan(Node* node) {
  FlagsContinuation cont(kUnorderedLessThan, node);
  VisitFloat64Compare(this, node, &cont);
}


void InstructionSelector::VisitFloat64LessThanOrEqual(Node* node) {
  FlagsContinuation cont(kUnorderedLessThanOrEqual, node);
  VisitFloat64Compare(this, node, &cont);
}

}  // namespace compiler
}  // namespace internal
}  // namespace v8
