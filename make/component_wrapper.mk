# Component wrapper makefile
#
# This makefile gets called recursively from the project make, once for each component.
# COMPONENT_MAKEFILE is set to point at the component.mk file for the component itself,
# which is included as part of this process (after default variables are defined).
#
# This makefile comprises multiple stages, marked in blocked comments below.
#
# CWD is the build directory of the component.

ifndef PROJECT_PATH
$(error Make was invoked from $(CURDIR). However please do not run make from the sdk or a component directory; invoke make from the project directory. See the ESP-IDF README for details.)
endif


################################################################################
# 1) Set default variables for the component build (including configuration
#	loaded from sdkconfig.)
################################################################################

# Find the path to the component
COMPONENT_PATH := $(abspath $(dir $(COMPONENT_MAKEFILE)))
export COMPONENT_PATH

# COMPONENT_BUILD_DIR is otherwise known as CWD for the build
COMPONENT_BUILD_DIR := $(abspath .)

# include elements common to both project & component makefiles
# (includes project configuration set via menuconfig)
include $(IDF_PATH)/make/common.mk

# Some of the following defaults may be overriden by the component's component.mk makefile,
# during the next step:

# Absolute path of the .a file
COMPONENT_LIBRARY = lib$(COMPONENT_NAME).a

# Source dirs a component has. Default to root directory of component.
COMPONENT_SRCDIRS = .

#Names of binary & text files to embed as raw content in the component library
COMPONENT_EMBED_FILES ?=
COMPONENT_EMBED_TXTFILES ?=

# By default, include only the include/ dir.
COMPONENT_ADD_INCLUDEDIRS = include
COMPONENT_ADD_LDFLAGS = -l$(COMPONENT_NAME)

# Define optional compiling macros
define compile_exclude 
COMPONENT_OBJEXCLUDE += $(1)
endef

define compile_include
COMPONENT_OBJINCLUDE += $(1)
endef

define compile_only_if
$(eval $(if $(1), $(call compile_include, $(2)), $(call compile_exclude, $(2))))
endef

define compile_only_if_not 
$(eval $(if $(1), $(call compile_exclude, $(2)), $(call compile_include, $(2))))
endef


################################################################################
# 2) Include the component.mk for the specific component (COMPONENT_MAKEFILE) to
#     override variables & optionally define custom targets.
################################################################################

include $(COMPONENT_MAKEFILE)


################################################################################
# 3) Set variables that depend on values that may changed by component.mk
################################################################################

# Object files which need to be linked into the library
# By default we take all .c, .cpp & .S files in COMPONENT_SRCDIRS.
ifndef COMPONENT_OBJS
# Find all source files in all COMPONENT_SRCDIRS
COMPONENT_OBJS := $(foreach compsrcdir,$(COMPONENT_SRCDIRS),$(patsubst %.c,%.o,$(wildcard $(COMPONENT_PATH)/$(compsrcdir)/*.c)))
COMPONENT_OBJS += $(foreach compsrcdir,$(COMPONENT_SRCDIRS),$(patsubst %.cpp,%.o,$(wildcard $(COMPONENT_PATH)/$(compsrcdir)/*.cpp)))
COMPONENT_OBJS += $(foreach compsrcdir,$(COMPONENT_SRCDIRS),$(patsubst %.S,%.o,$(wildcard $(COMPONENT_PATH)/$(compsrcdir)/*.S)))
# Make relative by removing COMPONENT_PATH from all found object paths
COMPONENT_OBJS := $(patsubst $(COMPONENT_PATH)/%,%,$(COMPONENT_OBJS))
else
# Add in components defined by conditional compiling macros
COMPONENT_OBJS += $(COMPONENT_OBJINCLUDE)
endif
# Remove items disabled by optional compilation
COMPONENT_OBJS := $(foreach obj,$(COMPONENT_OBJS),$(if $(filter $(realpath $(obj)),$(realpath $(COMPONENT_OBJEXCLUDE))), ,$(obj)))

# Remove duplicates
COMPONENT_OBJS := $(call uniq,$(COMPONENT_OBJS))


# Object files with embedded binaries to add to the component library
# Correspond to the files named in COMPONENT_EMBED_FILES & COMPONENT_EMBED_TXTFILES
COMPONENT_EMBED_OBJS ?= $(addsuffix .bin.o,$(notdir $(COMPONENT_EMBED_FILES))) $(addsuffix .txt.o,$(notdir $(COMPONENT_EMBED_TXTFILES)))

# If we're called to compile something, we'll get passed the COMPONENT_INCLUDES
# variable with all the include dirs from all the components in random order. This
# means we can accidentally grab a header from another component before grabbing our own.
# To make sure that does not happen, re-order the includes so ours come first.
OWN_INCLUDES:=$(abspath $(addprefix $(COMPONENT_PATH)/,$(COMPONENT_ADD_INCLUDEDIRS) $(COMPONENT_PRIV_INCLUDEDIRS)))
COMPONENT_INCLUDES := $(OWN_INCLUDES) $(filter-out $(OWN_INCLUDES),$(COMPONENT_INCLUDES))


################################################################################
# 4) Define a target to generate component_project_vars.mk Makefile which
# contains common per-component settings which are included directly in the
# top-level project make
################################################################################

# macro to generate variable-relative paths inside component_project_vars.mk, whenever possible
# ie put literal $(IDF_PATH), $(PROJECT_PATH) and $(BUILD_DIR_BASE) into the generated
# makefiles where possible.
#
# This means if directories move (breaking absolute paths), don't need to 'make clean'
define MakeVariablePath
$(subst $(IDF_PATH),$$(IDF_PATH),$(subst $(PROJECT_PATH),$$(PROJECT_PATH),$(subst $(BUILD_DIR_BASE),$$(BUILD_DIR_BASE),$(1))))
endef

# component_project_vars.mk target for the component. This is used to
# take component.mk variables COMPONENT_ADD_INCLUDEDIRS,
# COMPONENT_ADD_LDFLAGS, COMPONENT_DEPENDS and COMPONENT_SUBMODULES
# and inject those into the project make pass.
#
# The target here has no dependencies, as the parent target in
# project.mk evaluates dependencies before calling down to here. See
# GenerateComponentTargets macro in project.mk.
#
# If you are thinking of editing the output of this target for a
# component-specific feature, please don't! What you want is a
# Makefile.projbuild for your component (see docs/build-system.rst for
# more.)
component_project_vars.mk::
	$(details) "Building component project variables list $(abspath $@)"
	@echo '# Automatically generated build file. Do not edit.' > $@
	@echo 'COMPONENT_INCLUDES += $(call MakeVariablePath,$(addprefix $(COMPONENT_PATH)/,$(COMPONENT_ADD_INCLUDEDIRS)))' >> $@
	@echo 'COMPONENT_LDFLAGS += $(call MakeVariablePath,$(COMPONENT_ADD_LDFLAGS))' >> $@
	@echo 'COMPONENT_LINKER_DEPS += $(call MakeVariablePath,$(call resolvepath,$(COMPONENT_ADD_LINKER_DEPS),$(COMPONENT_PATH)))' >> $@
	@echo 'COMPONENT_SUBMODULES += $(call MakeVariablePath,$(addprefix $(COMPONENT_PATH)/,$(COMPONENT_SUBMODULES)))' >> $@
	@echo '$(COMPONENT_NAME)-build: $(addsuffix -build,$(COMPONENT_DEPENDS))' >> $@


################################################################################
# 5) If COMPONENT_OWNBUILDTARGET / COMPONENT_OWNCLEANTARGET is not set by component.mk,
#	define default build, clean, etc. targets
################################################################################

# If COMPONENT_OWNBUILDTARGET is not set, define a phony build target and
# a COMPONENT_LIBRARY link target.
ifndef COMPONENT_OWNBUILDTARGET
.PHONY: build
build: $(COMPONENT_LIBRARY)
	@mkdir -p $(COMPONENT_SRCDIRS)

# Build the archive. We remove the archive first, otherwise ar will get confused if we update
# an archive when multiple filenames have the same name (src1/test.o and src2/test.o)
$(COMPONENT_LIBRARY): $(COMPONENT_OBJS) $(COMPONENT_EMBED_OBJS)
	$(summary) AR $@
	rm -f $@
	$(AR) cru $@ $^
endif

# If COMPONENT_OWNCLEANTARGET is not set, define a phony clean target
ifndef COMPONENT_OWNCLEANTARGET
CLEAN_FILES := $(COMPONENT_LIBRARY) $(COMPONENT_OBJS) $(COMPONENT_OBJS:.o=.d) $(COMPONENT_OBJEXCLUDE) $(COMPONENT_OBJEXCLUDE:.o=.d) $(COMPONENT_EMBED_OBJS) $(COMPONENT_EXTRA_CLEAN) component_project_vars.mk
.PHONY: clean
clean:
	$(summary) RM $(CLEAN_FILES)
	rm -f $(CLEAN_FILES)
endif

# Include all dependency files already generated
-include $(COMPONENT_OBJS:.o=.d)

# This pattern is generated for each COMPONENT_SRCDIR to compile the files in it.
define GenerateCompileTargets
# $(1) - directory containing source files, relative to $(COMPONENT_PATH)
$(1)/%.o: $$(COMPONENT_PATH)/$(1)/%.c $(COMMON_MAKEFILES) $(COMPONENT_MAKEFILE) | $(1)
	$$(summary) CC $$@
	$$(CC) $$(CFLAGS) $$(CPPFLAGS) $$(addprefix -I ,$$(COMPONENT_INCLUDES)) $$(addprefix -I ,$$(COMPONENT_EXTRA_INCLUDES)) -I$(1) -c $$< -o $$@

$(1)/%.o: $$(COMPONENT_PATH)/$(1)/%.cpp $(COMMON_MAKEFILES) $(COMPONENT_MAKEFILE) | $(1)
	$$(summary) CXX $$@
	$$(CXX) $$(CXXFLAGS) $$(CPPFLAGS) $$(addprefix -I,$$(COMPONENT_INCLUDES)) $$(addprefix -I,$$(COMPONENT_EXTRA_INCLUDES)) -I$(1) -c $$< -o $$@

$(1)/%.o: $$(COMPONENT_PATH)/$(1)/%.S $(COMMON_MAKEFILES) $(COMPONENT_MAKEFILE) | $(1)
	$$(summary) AS $$@
	$$(CC) $$(CPPFLAGS) $$(DEBUG_FLAGS) $$(addprefix -I ,$$(COMPONENT_INCLUDES)) $$(addprefix -I ,$$(COMPONENT_EXTRA_INCLUDES)) -I$(1) -c $$< -o $$@

# CWD is build dir, create the build subdirectory if it doesn't exist
$(1):
	@mkdir -p $(1)
endef

# Generate all the compile target patterns
$(foreach srcdir,$(COMPONENT_SRCDIRS), $(eval $(call GenerateCompileTargets,$(srcdir))))

## Support for embedding binary files into the ELF as symbols

OBJCOPY_EMBED_ARGS := --input-target binary --output-target elf32-xtensa-le --binary-architecture xtensa --rename-section .data=.rodata.embedded

# Generate pattern for embedding text or binary files into the app
# $(1) is name of file (as relative path inside component)
# $(2) is txt or bin depending on file contents
#
# txt files are null-terminated before being embedded (otherwise
# identical behaviour.)
#
define GenerateEmbedTarget

# copy the input file into the build dir (using a subdirectory
# in case the file already exists elsewhere in the build dir)
embed_bin/$$(notdir $(1)): $(call resolvepath,$(1),$(COMPONENT_PATH)) | embed_bin
	cp $$< $$@

embed_txt/$$(notdir $(1)): $(call resolvepath,$(1),$(COMPONENT_PATH)) | embed_txt
	cp $$< $$@
	printf '\0' >> $$@  # null-terminate text files

# messing about with the embed_X subdirectory then using 'cd' for objcopy is because the
# full path passed to OBJCOPY makes it into the name of the symbols in the .o file
$$(notdir $(1)).$(2).o: embed_$(2)/$$(notdir $(1))
	$(summary) EMBED $$@
	cd embed_$(2); $(OBJCOPY) $(OBJCOPY_EMBED_ARGS) $$(notdir $$<) ../$$@

CLEAN_FILES += embed_$(2)/$$(notdir $(1))
endef

embed_txt embed_bin:
	mkdir -p $@

# generate targets to embed binary & text files
$(foreach binfile,$(COMPONENT_EMBED_FILES), $(eval $(call GenerateEmbedTarget,$(binfile),bin)))

$(foreach txtfile,$(COMPONENT_EMBED_TXTFILES), $(eval $(call GenerateEmbedTarget,$(txtfile),txt)))

# generate targets to create binary embed directories
$(foreach bindir,$(sort $(dir $(COMPONENT_EMBED_FILES))), $(eval $(call GenerateBuildDirTarget,$(bindir))))
