# This file is generated by gyp; do not edit.

TOOLSET := target
TARGET := v8_libbase
DEFS_Debug := \
	'-DV8_TARGET_ARCH_X64' \
	'-DENABLE_GDB_JIT_INTERFACE' \
	'-DV8_DEPRECATION_WARNINGS' \
	'-DV8_I18N_SUPPORT' \
	'-DENABLE_DISASSEMBLER' \
	'-DV8_ENABLE_CHECKS' \
	'-DOBJECT_PRINT' \
	'-DVERIFY_HEAP' \
	'-DDEBUG' \
	'-DENABLE_EXTRA_CHECKS' \
	'-DENABLE_HANDLE_ZAPPING'

# Flags passed to all source files.
CFLAGS_Debug := \
	-O0 \
	-gdwarf-2 \
	-fvisibility=hidden \
	-Werror \
	-mmacosx-version-min=10.5 \
	-arch x86_64 \
	-Wall \
	-Wendif-labels \
	-W \
	-Wno-unused-parameter \
	-Wno-missing-field-initializers

# Flags passed to only C files.
CFLAGS_C_Debug := \
	-std=c99 \
	-fno-strict-aliasing

# Flags passed to only C++ files.
CFLAGS_CC_Debug := \
	-std=gnu++0x \
	-fno-rtti \
	-fno-exceptions \
	-fvisibility-inlines-hidden \
	-fno-threadsafe-statics \
	-fno-strict-aliasing

# Flags passed to only ObjC files.
CFLAGS_OBJC_Debug :=

# Flags passed to only ObjC++ files.
CFLAGS_OBJCC_Debug :=

INCS_Debug := \
	-I$(srcdir)/.

DEFS_Optdebug := \
	'-DV8_TARGET_ARCH_X64' \
	'-DENABLE_GDB_JIT_INTERFACE' \
	'-DV8_DEPRECATION_WARNINGS' \
	'-DV8_I18N_SUPPORT' \
	'-DENABLE_DISASSEMBLER' \
	'-DV8_ENABLE_CHECKS' \
	'-DOBJECT_PRINT' \
	'-DVERIFY_HEAP' \
	'-DDEBUG' \
	'-DENABLE_EXTRA_CHECKS' \
	'-DENABLE_HANDLE_ZAPPING'

# Flags passed to all source files.
CFLAGS_Optdebug := \
	-O3 \
	-gdwarf-2 \
	-fstrict-aliasing \
	-fvisibility=hidden \
	-Werror \
	-mmacosx-version-min=10.5 \
	-arch x86_64 \
	-Wall \
	-Wendif-labels \
	-W \
	-Wno-unused-parameter \
	-Wno-missing-field-initializers

# Flags passed to only C files.
CFLAGS_C_Optdebug := \
	-std=c99 \
	-fno-strict-aliasing

# Flags passed to only C++ files.
CFLAGS_CC_Optdebug := \
	-std=gnu++0x \
	-fno-rtti \
	-fno-exceptions \
	-fvisibility-inlines-hidden \
	-fno-threadsafe-statics \
	-fno-strict-aliasing

# Flags passed to only ObjC files.
CFLAGS_OBJC_Optdebug :=

# Flags passed to only ObjC++ files.
CFLAGS_OBJCC_Optdebug :=

INCS_Optdebug := \
	-I$(srcdir)/.

DEFS_Release := \
	'-DV8_TARGET_ARCH_X64' \
	'-DENABLE_GDB_JIT_INTERFACE' \
	'-DV8_DEPRECATION_WARNINGS' \
	'-DV8_I18N_SUPPORT' \
	'-DENABLE_HANDLE_ZAPPING'

# Flags passed to all source files.
CFLAGS_Release := \
	-O3 \
	-gdwarf-2 \
	-fstrict-aliasing \
	-fvisibility=hidden \
	-Werror \
	-mmacosx-version-min=10.5 \
	-arch x86_64 \
	-Wall \
	-Wendif-labels \
	-W \
	-Wno-unused-parameter \
	-Wno-missing-field-initializers

# Flags passed to only C files.
CFLAGS_C_Release := \
	-std=c99 \
	-fno-strict-aliasing

# Flags passed to only C++ files.
CFLAGS_CC_Release := \
	-std=gnu++0x \
	-fno-rtti \
	-fno-exceptions \
	-fvisibility-inlines-hidden \
	-fno-threadsafe-statics \
	-fno-strict-aliasing

# Flags passed to only ObjC files.
CFLAGS_OBJC_Release :=

# Flags passed to only ObjC++ files.
CFLAGS_OBJCC_Release :=

INCS_Release := \
	-I$(srcdir)/.

OBJS := \
	$(obj).target/$(TARGET)/src/base/atomicops_internals_x86_gcc.o \
	$(obj).target/$(TARGET)/src/base/bits.o \
	$(obj).target/$(TARGET)/src/base/cpu.o \
	$(obj).target/$(TARGET)/src/base/division-by-constant.o \
	$(obj).target/$(TARGET)/src/base/functional.o \
	$(obj).target/$(TARGET)/src/base/logging.o \
	$(obj).target/$(TARGET)/src/base/once.o \
	$(obj).target/$(TARGET)/src/base/platform/time.o \
	$(obj).target/$(TARGET)/src/base/platform/condition-variable.o \
	$(obj).target/$(TARGET)/src/base/platform/mutex.o \
	$(obj).target/$(TARGET)/src/base/platform/semaphore.o \
	$(obj).target/$(TARGET)/src/base/sys-info.o \
	$(obj).target/$(TARGET)/src/base/utils/random-number-generator.o \
	$(obj).target/$(TARGET)/src/base/platform/platform-macos.o \
	$(obj).target/$(TARGET)/src/base/platform/platform-posix.o

# Add to the list of files we specially track dependencies for.
all_deps += $(OBJS)

# CFLAGS et al overrides must be target-local.
# See "Target-specific Variable Values" in the GNU Make manual.
$(OBJS): TOOLSET := $(TOOLSET)
$(OBJS): GYP_CFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_C_$(BUILDTYPE))
$(OBJS): GYP_CXXFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_CC_$(BUILDTYPE))
$(OBJS): GYP_OBJCFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_C_$(BUILDTYPE)) $(CFLAGS_OBJC_$(BUILDTYPE))
$(OBJS): GYP_OBJCXXFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_CC_$(BUILDTYPE)) $(CFLAGS_OBJCC_$(BUILDTYPE))

# Suffix rules, putting all outputs into $(obj).

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(srcdir)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

# Try building from generated source, too.

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj).$(TOOLSET)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

# End of this set of suffix rules
### Rules for final target.
LDFLAGS_Debug := \
	-mmacosx-version-min=10.5 \
	-arch x86_64 \
	-L$(builddir)

LIBTOOLFLAGS_Debug :=

LDFLAGS_Optdebug := \
	-mmacosx-version-min=10.5 \
	-arch x86_64 \
	-L$(builddir)

LIBTOOLFLAGS_Optdebug :=

LDFLAGS_Release := \
	-mmacosx-version-min=10.5 \
	-arch x86_64 \
	-L$(builddir)

LIBTOOLFLAGS_Release :=

LIBS :=

$(builddir)/libv8_libbase.a: GYP_LDFLAGS := $(LDFLAGS_$(BUILDTYPE))
$(builddir)/libv8_libbase.a: LIBS := $(LIBS)
$(builddir)/libv8_libbase.a: GYP_LIBTOOLFLAGS := $(LIBTOOLFLAGS_$(BUILDTYPE))
$(builddir)/libv8_libbase.a: TOOLSET := $(TOOLSET)
$(builddir)/libv8_libbase.a: $(OBJS) FORCE_DO_CMD
	$(call do_cmd,alink)

all_deps += $(builddir)/libv8_libbase.a
# Add target alias
.PHONY: v8_libbase
v8_libbase: $(builddir)/libv8_libbase.a

# Add target alias to "all" target.
.PHONY: all
all: v8_libbase

