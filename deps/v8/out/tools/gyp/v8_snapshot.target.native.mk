# This file is generated by gyp; do not edit.

TOOLSET := target
TARGET := v8_snapshot
### Rules for action "run_mksnapshot":
quiet_cmd_tools_gyp_v8_gyp_v8_snapshot_target_run_mksnapshot = ACTION tools_gyp_v8_gyp_v8_snapshot_target_run_mksnapshot $@
cmd_tools_gyp_v8_gyp_v8_snapshot_target_run_mksnapshot = LD_LIBRARY_PATH=$(builddir)/lib.host:$(builddir)/lib.target:$$LD_LIBRARY_PATH; export LD_LIBRARY_PATH; cd $(srcdir)/tools/gyp; mkdir -p $(obj).$(TOOLSET)/v8_snapshot/geni; "$(builddir)/mksnapshot" --log-snapshot-positions --logfile "$(obj).$(TOOLSET)/v8_snapshot/geni/snapshot.log" --random-seed 314159265 "$(obj).$(TOOLSET)/v8_snapshot/geni/snapshot.cc"

$(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc: obj := $(abs_obj)
$(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc: builddir := $(abs_builddir)
$(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc: export BUILT_PRODUCTS_DIR := ${abs_builddir}
$(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc: export CONFIGURATION := ${BUILDTYPE}
$(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc: export EXECUTABLE_NAME := libv8_snapshot.a
$(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc: export EXECUTABLE_PATH := libv8_snapshot.a
$(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc: export FULL_PRODUCT_NAME := libv8_snapshot.a
$(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc: export MACH_O_TYPE := staticlib
$(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc: export PRODUCT_NAME := v8_snapshot
$(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc: export PRODUCT_TYPE := com.apple.product-type.library.static
$(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc: export SDKROOT := 
$(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc: export SRCROOT := ${abs_srcdir}/tools/gyp
$(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc: export SOURCE_ROOT := ${SRCROOT}
$(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc: export TARGET_BUILD_DIR := ${abs_builddir}
$(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc: export TEMP_DIR := ${TMPDIR}
$(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc: TOOLSET := $(TOOLSET)
$(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc: $(builddir)/mksnapshot FORCE_DO_CMD
	$(call do_cmd,tools_gyp_v8_gyp_v8_snapshot_target_run_mksnapshot)

all_deps += $(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc
action_tools_gyp_v8_gyp_v8_snapshot_target_run_mksnapshot_outputs := $(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.cc


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
	$(obj).target/$(TARGET)/gen/libraries.o \
	$(obj).target/$(TARGET)/gen/experimental-libraries.o \
	$(obj).$(TOOLSET)/$(TARGET)/geni/snapshot.o \
	$(obj).target/$(TARGET)/src/snapshot-common.o

# Add to the list of files we specially track dependencies for.
all_deps += $(OBJS)

# Make sure our dependencies are built before any of us.
$(OBJS): | $(builddir)/mksnapshot $(obj).target/tools/gyp/js2c.stamp

# Make sure our actions/rules run before any of us.
$(OBJS): | $(action_tools_gyp_v8_gyp_v8_snapshot_target_run_mksnapshot_outputs)

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
# Build our special outputs first.
$(builddir)/libv8_snapshot.a: | $(action_tools_gyp_v8_gyp_v8_snapshot_target_run_mksnapshot_outputs)

# Preserve order dependency of special output on deps.
$(action_tools_gyp_v8_gyp_v8_snapshot_target_run_mksnapshot_outputs): | $(builddir)/mksnapshot $(obj).target/tools/gyp/js2c.stamp

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

$(builddir)/libv8_snapshot.a: GYP_LDFLAGS := $(LDFLAGS_$(BUILDTYPE))
$(builddir)/libv8_snapshot.a: LIBS := $(LIBS)
$(builddir)/libv8_snapshot.a: GYP_LIBTOOLFLAGS := $(LIBTOOLFLAGS_$(BUILDTYPE))
$(builddir)/libv8_snapshot.a: TOOLSET := $(TOOLSET)
$(builddir)/libv8_snapshot.a: $(OBJS) FORCE_DO_CMD
	$(call do_cmd,alink)

all_deps += $(builddir)/libv8_snapshot.a
# Add target alias
.PHONY: v8_snapshot
v8_snapshot: $(builddir)/libv8_snapshot.a

# Add target alias to "all" target.
.PHONY: all
all: v8_snapshot

