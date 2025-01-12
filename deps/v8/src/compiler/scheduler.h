// Copyright 2013 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_COMPILER_SCHEDULER_H_
#define V8_COMPILER_SCHEDULER_H_

#include "src/v8.h"

#include "src/compiler/opcodes.h"
#include "src/compiler/schedule.h"
#include "src/compiler/zone-pool.h"
#include "src/zone-containers.h"

namespace v8 {
namespace internal {
namespace compiler {

// Computes a schedule from a graph, placing nodes into basic blocks and
// ordering the basic blocks in the special RPO order.
class Scheduler {
 public:
  // The complete scheduling algorithm. Creates a new schedule and places all
  // nodes from the graph into it.
  static Schedule* ComputeSchedule(ZonePool* zone_pool, Graph* graph);

  // Compute the RPO of blocks in an existing schedule.
  static BasicBlockVector* ComputeSpecialRPO(ZonePool* zone_pool,
                                             Schedule* schedule);

 private:
  enum Placement { kUnknown, kSchedulable, kFixed };

  // Per-node data tracked during scheduling.
  struct SchedulerData {
    BasicBlock* minimum_block_;  // Minimum legal RPO placement.
    int unscheduled_count_;      // Number of unscheduled uses of this node.
    bool is_connected_control_;  // {true} if control-connected to the end node.
    bool is_floating_control_;   // {true} if control, but not control-connected
                                 // to the end node.
    Placement placement_ : 3;    // Whether the node is fixed, schedulable,
                                 // or not yet known.
  };

  ZonePool* zone_pool_;
  Zone* zone_;
  Graph* graph_;
  Schedule* schedule_;
  NodeVectorVector scheduled_nodes_;
  NodeVector schedule_root_nodes_;
  ZoneVector<SchedulerData> node_data_;
  bool has_floating_control_;

  Scheduler(ZonePool* zone_pool, Zone* zone, Graph* graph, Schedule* schedule);

  SchedulerData DefaultSchedulerData();

  SchedulerData* GetData(Node* node) {
    DCHECK(node->id() < static_cast<int>(node_data_.size()));
    return &node_data_[node->id()];
  }

  Placement GetPlacement(Node* node);

  int GetRPONumber(BasicBlock* block) {
    DCHECK(block->rpo_number() >= 0 &&
           block->rpo_number() <
               static_cast<int>(schedule_->rpo_order_.size()));
    DCHECK(schedule_->rpo_order_[block->rpo_number()] == block);
    return block->rpo_number();
  }

  BasicBlock* GetCommonDominator(BasicBlock* b1, BasicBlock* b2);

  // Phase 1: Build control-flow graph and dominator tree.
  friend class CFGBuilder;
  void BuildCFG();
  void GenerateImmediateDominatorTree();

  // Phase 2: Prepare use counts for nodes.
  friend class PrepareUsesVisitor;
  void PrepareUses();

  // Phase 3: Schedule nodes early.
  friend class ScheduleEarlyNodeVisitor;
  void ScheduleEarly();

  // Phase 4: Schedule nodes late.
  friend class ScheduleLateNodeVisitor;
  void ScheduleLate();

  bool ConnectFloatingControl();
  void ConnectFloatingControlSubgraph(BasicBlock* block, Node* node);
};

}  // namespace compiler
}  // namespace internal
}  // namespace v8

#endif  // V8_COMPILER_SCHEDULER_H_
