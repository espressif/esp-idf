#
# Main Project Makefile
# This Makefile is included directly from the user project Makefile in order to call the component.mk
# makefiles of all components (in a separate make process) to build all the libraries, then links them
# together into the final file. If so, PWD is the project dir (we assume).
#

#
# This makefile requires the environment variable IDF_PATH to be set to the top-level esp-idf directory
# where this file is located.
#

.PHONY: build-components menuconfig defconfig all build clean all_binaries check-submodules size
all: all_binaries
# see below for recipe of 'all' target
#
# # other components will add dependencies to 'all_binaries'. The
# reason all_binaries is used instead of 'all' is so that the flash
# target can build everything without triggering the per-component "to
# flash..." output targets.)

help:
	@echo "Welcome to Espressif IDF build system. Some useful make targets:"
	@echo ""
	@echo "make menuconfig - Configure IDF project"
	@echo "make defconfig - Set defaults for all new configuration options"
	@echo ""
	@echo "make all - Build app, bootloader, partition table"
	@echo "make flash - Flash all components to a fresh chip"
	@echo "make clean - Remove all build output"
	@echo "make size - Display the memory footprint of the app"
	@echo ""
	@echo "make app - Build just the app"
	@echo "make app-flash - Flash just the app"
	@echo "make app-clean - Clean just the app"
	@echo ""
	@echo "See also 'make bootloader', 'make bootloader-flash', 'make bootloader-clean', "
	@echo "'make partition_table', etc, etc."

# dependency checks
ifndef MAKE_RESTARTS
ifeq ("$(filter 4.% 3.81 3.82,$(MAKE_VERSION))","")
$(warning "esp-idf build system only supports GNU Make versions 3.81 or newer. You may see unexpected results with other Makes.")
endif
endif

# disable built-in make rules, makes debugging saner
MAKEFLAGS_OLD := $(MAKEFLAGS)
MAKEFLAGS +=-rR

# Default path to the project: we assume the Makefile including this file
# is in the project directory
ifndef PROJECT_PATH
PROJECT_PATH := $(abspath $(dir $(firstword $(MAKEFILE_LIST))))
export PROJECT_PATH
endif

# A list of the "common" makefiles, to use as a target dependency
COMMON_MAKEFILES := $(abspath $(IDF_PATH)/make/project.mk $(IDF_PATH)/make/common.mk $(IDF_PATH)/make/component_wrapper.mk)
export COMMON_MAKEFILES

# The directory where we put all objects/libraries/binaries. The project Makefile can
# configure this if needed.
BUILD_DIR_BASE ?= $(PROJECT_PATH)/build
export BUILD_DIR_BASE

# Component directories. These directories are searched for components.
# The project Makefile can override these component dirs, or define extra component directories.
COMPONENT_DIRS ?= $(PROJECT_PATH)/components $(EXTRA_COMPONENT_DIRS) $(IDF_PATH)/components
export COMPONENT_DIRS

# Source directories of the project itself (a special, project-specific component.) Defaults to only "main".
SRCDIRS ?= main

# The project Makefile can define a list of components, but if it does not do this we just take
# all available components in the component dirs.
ifndef COMPONENTS
# Find all component names. The component names are the same as the
# directories they're in, so /bla/components/mycomponent/ -> mycomponent. We then use
# COMPONENT_DIRS to build COMPONENT_PATHS with the full path to each component.
COMPONENTS := $(foreach dir,$(COMPONENT_DIRS),$(wildcard $(dir)/*))
COMPONENTS := $(sort $(foreach comp,$(COMPONENTS),$(lastword $(subst /, ,$(comp)))))
endif
export COMPONENTS

# Resolve all of COMPONENTS into absolute paths in COMPONENT_PATHS.
#
# If a component name exists in multiple COMPONENT_DIRS, we take the first match.
#
# NOTE: These paths must be generated WITHOUT a trailing / so we
# can use $(notdir x) to get the component name.
COMPONENT_PATHS := $(foreach comp,$(COMPONENTS),$(firstword $(foreach dir,$(COMPONENT_DIRS),$(wildcard $(dir)/$(comp)))))
COMPONENT_PATHS += $(abspath $(SRCDIRS))

# A component is buildable if it has a component.mk makefile in it
COMPONENT_PATHS_BUILDABLE := $(foreach cp,$(COMPONENT_PATHS),$(if $(wildcard $(cp)/component.mk),$(cp)))

# If TESTS_ALL set to 1, set TEST_COMPONENTS to all components
ifeq ($(TESTS_ALL),1)
TEST_COMPONENTS := $(COMPONENTS)
endif

# If TEST_COMPONENTS is set, create variables for building unit tests
ifdef TEST_COMPONENTS
override TEST_COMPONENTS := $(foreach comp,$(TEST_COMPONENTS),$(wildcard $(IDF_PATH)/components/$(comp)/test))
TEST_COMPONENT_PATHS := $(TEST_COMPONENTS)
TEST_COMPONENT_NAMES :=  $(foreach comp,$(TEST_COMPONENTS),$(lastword $(subst /, ,$(dir $(comp))))_test)
endif

# Initialise project-wide variables which can be added to by
# each component.
#
# These variables are built up via the component_project_vars.mk
# generated makefiles (one per component).
#
# See docs/build-system.rst for more details.
COMPONENT_INCLUDES :=
COMPONENT_LDFLAGS :=
COMPONENT_SUBMODULES :=

# COMPONENT_PROJECT_VARS is the list of component_project_vars.mk generated makefiles
# for each component.
#
# Including $(COMPONENT_PROJECT_VARS) builds the COMPONENT_INCLUDES,
# COMPONENT_LDFLAGS variables and also targets for any inter-component
# dependencies.
#
# See the component_project_vars.mk target in component_wrapper.mk
COMPONENT_PROJECT_VARS := $(addsuffix /component_project_vars.mk,$(notdir $(COMPONENT_PATHS_BUILDABLE) ) $(TEST_COMPONENT_NAMES))
COMPONENT_PROJECT_VARS := $(addprefix $(BUILD_DIR_BASE)/,$(COMPONENT_PROJECT_VARS))
# this line is -include instead of include to prevent a spurious error message on make 3.81
-include $(COMPONENT_PROJECT_VARS)

# Also add top-level project include path, for top-level includes
COMPONENT_INCLUDES += $(abspath $(BUILD_DIR_BASE)/include/)

export COMPONENT_INCLUDES

# Set variables common to both project & component
include $(IDF_PATH)/make/common.mk

all:
ifdef CONFIG_SECURE_BOOT_ENABLED
	@echo "(Secure boot enabled, so bootloader not flashed automatically. See 'make bootloader' output)"
ifndef CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES
	@echo "App built but not signed. Sign app & partition data before flashing, via espsecure.py:"
	@echo "espsecure.py sign_data --keyfile KEYFILE $(APP_BIN)"
	@echo "espsecure.py sign_data --keyfile KEYFILE $(PARTITION_TABLE_BIN)"
endif
	@echo "To flash app & partition table, run 'make flash' or:"
else
	@echo "To flash all build output, run 'make flash' or:"
endif
	@echo $(ESPTOOLPY_WRITE_FLASH) $(ESPTOOL_ALL_FLASH_ARGS)

# Set default LDFLAGS

LDFLAGS ?= -nostdlib \
	$(addprefix -L$(BUILD_DIR_BASE)/,$(COMPONENTS) $(TEST_COMPONENT_NAMES) $(SRCDIRS) ) \
	-u call_user_start_cpu0	\
	$(EXTRA_LDFLAGS) \
	-Wl,--gc-sections	\
	-Wl,-static	\
	-Wl,--start-group	\
	$(COMPONENT_LDFLAGS) \
	-lgcc \
	-Wl,--end-group \
	-Wl,-EL

# Set default CPPFLAGS, CFLAGS, CXXFLAGS
# These are exported so that components can use them when compiling.
# If you need your component to add CFLAGS/etc for it's own source compilation only, set CFLAGS += in your component's Makefile.
# If you need your component to add CFLAGS/etc globally for all source
#  files, set CFLAGS += in your component's Makefile.projbuild
# If you need to set CFLAGS/CPPFLAGS/CXXFLAGS at project level, set them in application Makefile
#  before including project.mk. Default flags will be added before the ones provided in application Makefile.

# CPPFLAGS used by C preprocessor
# If any flags are defined in application Makefile, add them at the end. 
CPPFLAGS := -DESP_PLATFORM -MMD -MP $(CPPFLAGS) $(EXTRA_CPPFLAGS)

# Warnings-related flags relevant both for C and C++
COMMON_WARNING_FLAGS = -Wall -Werror=all \
	-Wno-error=unused-function \
	-Wno-error=unused-but-set-variable \
	-Wno-error=unused-variable \
	-Wno-error=deprecated-declarations \
	-Wextra \
	-Wno-unused-parameter -Wno-sign-compare

# Flags which control code generation and dependency generation, both for C and C++
COMMON_FLAGS = \
	-ffunction-sections -fdata-sections \
	-fstrict-volatile-bitfields \
	-mlongcalls \
	-nostdlib

# Optimization flags are set based on menuconfig choice
ifneq ("$(CONFIG_OPTIMIZATION_LEVEL_RELEASE)","")
OPTIMIZATION_FLAGS = -Os
CPPFLAGS += -DNDEBUG
else
OPTIMIZATION_FLAGS = -Og
endif

# Enable generation of debugging symbols
OPTIMIZATION_FLAGS += -ggdb

# List of flags to pass to C compiler
# If any flags are defined in application Makefile, add them at the end.
CFLAGS := $(strip \
	-std=gnu99 \
	$(OPTIMIZATION_FLAGS) \
	$(COMMON_FLAGS) \
	$(COMMON_WARNING_FLAGS) -Wno-old-style-declaration \
	$(CFLAGS) \
	$(EXTRA_CFLAGS))

# List of flags to pass to C++ compiler
# If any flags are defined in application Makefile, add them at the end.
CXXFLAGS := $(strip \
	-std=gnu++11 \
	-fno-exceptions \
	-fno-rtti \
	$(OPTIMIZATION_FLAGS) \
	$(COMMON_FLAGS) \
	$(COMMON_WARNING_FLAGS) \
	$(CXXFLAGS) \
	$(EXTRA_CXXFLAGS))

export CFLAGS CPPFLAGS CXXFLAGS

# Set host compiler and binutils
HOSTCC := $(CC)
HOSTLD := $(LD)
HOSTAR := $(AR)
HOSTOBJCOPY := $(OBJCOPY)
HOSTSIZE := $(SIZE)
export HOSTCC HOSTLD HOSTAR HOSTOBJCOPY SIZE

# Set target compiler. Defaults to whatever the user has
# configured as prefix + ye olde gcc commands
CC := $(call dequote,$(CONFIG_TOOLPREFIX))gcc
CXX := $(call dequote,$(CONFIG_TOOLPREFIX))c++
LD := $(call dequote,$(CONFIG_TOOLPREFIX))ld
AR := $(call dequote,$(CONFIG_TOOLPREFIX))ar
OBJCOPY := $(call dequote,$(CONFIG_TOOLPREFIX))objcopy
SIZE := $(call dequote,$(CONFIG_TOOLPREFIX))size
export CC CXX LD AR OBJCOPY SIZE

PYTHON=$(call dequote,$(CONFIG_PYTHON))

# the app is the main executable built by the project
APP_ELF:=$(BUILD_DIR_BASE)/$(PROJECT_NAME).elf
APP_MAP:=$(APP_ELF:.elf=.map)
APP_BIN:=$(APP_ELF:.elf=.bin)

# Include any Makefile.projbuild file letting components add
# configuration at the project level
define includeProjBuildMakefile
$(if $(V),$(if $(wildcard $(1)/Makefile.projbuild),$(info including $(1)/Makefile.projbuild...)))
COMPONENT_PATH := $(1)
-include $(1)/Makefile.projbuild
endef
$(foreach componentpath,$(COMPONENT_PATHS),$(eval $(call includeProjBuildMakefile,$(componentpath))))

# once we know component paths, we can include the config generation targets
#
# (bootloader build doesn't need this, config is exported from top-level)
ifndef IS_BOOTLOADER_BUILD
include $(IDF_PATH)/make/project_config.mk
endif

# A "component" library is any library in the LDFLAGS where
# the name of the library is also a name of the component
APP_LIBRARIES = $(patsubst -l%,%,$(filter -l%,$(LDFLAGS)))
COMPONENT_LIBRARIES = $(filter $(notdir $(COMPONENT_PATHS_BUILDABLE)) $(TEST_COMPONENT_NAMES),$(APP_LIBRARIES))

# ELF depends on the library archive files for COMPONENT_LIBRARIES
# the rules to build these are emitted as part of GenerateComponentTarget below
#
# also depends on additional dependencies (linker scripts & binary libraries)
# stored in COMPONENT_LINKER_DEPS, built via component.mk files' COMPONENT_ADD_LINKER_DEPS variable
$(APP_ELF): $(foreach libcomp,$(COMPONENT_LIBRARIES),$(BUILD_DIR_BASE)/$(libcomp)/lib$(libcomp).a) $(COMPONENT_LINKER_DEPS)
	$(summary) LD $(notdir $@)
	$(CC) $(LDFLAGS) -o $@ -Wl,-Map=$(APP_MAP)

# Generation of $(APP_BIN) from $(APP_ELF) is added by the esptool
# component's Makefile.projbuild
app: $(APP_BIN)
ifeq ("$(CONFIG_SECURE_BOOT_ENABLED)$(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)","y") # secure boot enabled, but remote sign app image
	@echo "App built but not signed. Signing step via espsecure.py:"
	@echo "espsecure.py sign_data --keyfile KEYFILE $(APP_BIN)"
	@echo "Then flash app command is:"
	@echo $(ESPTOOLPY_WRITE_FLASH) $(CONFIG_APP_OFFSET) $(APP_BIN)
else
	@echo "App built. Default flash app command is:"
	@echo $(ESPTOOLPY_WRITE_FLASH) $(CONFIG_APP_OFFSET) $(APP_BIN)
endif

all_binaries: $(APP_BIN)

$(BUILD_DIR_BASE):
	mkdir -p $(BUILD_DIR_BASE)

# Macro for the recursive sub-make for each component
# $(1) - component directory
# $(2) - component name only
#
# Is recursively expanded by the GenerateComponentTargets macro
define ComponentMake
+$(MAKE) -C $(BUILD_DIR_BASE)/$(2) -f $(IDF_PATH)/make/component_wrapper.mk COMPONENT_MAKEFILE=$(1)/component.mk COMPONENT_NAME=$(2)
endef

# Generate top-level component-specific targets for each component
# $(1) - path to component dir
# $(2) - name of component
#
define GenerateComponentTargets
.PHONY: $(2)-build $(2)-clean

$(2)-build: check-submodules
	$(call ComponentMake,$(1),$(2)) build

$(2)-clean:
	$(call ComponentMake,$(1),$(2)) clean

$(BUILD_DIR_BASE)/$(2):
	@mkdir -p $(BUILD_DIR_BASE)/$(2)

# tell make it can build any component's library by invoking the -build target
# (this target exists for all components even ones which don't build libraries, but it's
# only invoked for the targets whose libraries appear in COMPONENT_LIBRARIES and hence the
# APP_ELF dependencies.)
$(BUILD_DIR_BASE)/$(2)/lib$(2).a: $(2)-build
	$(details) "Target '$$^' responsible for '$$@'" # echo which build target built this file

# add a target to generate the component_project_vars.mk files that
# are used to inject variables into project make pass (see matching
# component_project_vars.mk target in component_wrapper.mk).
#
# If any component_project_vars.mk file is out of date, the make
# process will call this target to rebuild it and then restart.
#
# Note: $(SDKCONFIG) is a normal prereq as we need to rebuild these
# files whenever the config changes. $(SDKCONFIG_MAKEFILE) is an
# order-only prereq because if it hasn't been rebuilt, we need to
# build it first - but including it as a normal prereq can lead to
# infinite restarts as the conf process will keep updating it.
$(BUILD_DIR_BASE)/$(2)/component_project_vars.mk: $(1)/component.mk $(COMMON_MAKEFILES) $(SDKCONFIG) | $(BUILD_DIR_BASE)/$(2) $(SDKCONFIG_MAKEFILE)
	$(call ComponentMake,$(1),$(2)) component_project_vars.mk
endef

$(foreach component,$(COMPONENT_PATHS_BUILDABLE),$(eval $(call GenerateComponentTargets,$(component),$(notdir $(component)))))
$(foreach component,$(TEST_COMPONENT_PATHS),$(eval $(call GenerateComponentTargets,$(component),$(lastword $(subst /, ,$(dir $(component))))_test)))

app-clean: $(addsuffix -clean,$(notdir $(COMPONENT_PATHS_BUILDABLE)))
	$(summary) RM $(APP_ELF)
	rm -f $(APP_ELF) $(APP_BIN) $(APP_MAP)
	
size: $(APP_ELF)
	$(SIZE) $(APP_ELF)

# NB: this ordering is deliberate (app-clean before config-clean),
# so config remains valid during all component clean targets
config-clean: app-clean
clean: config-clean

# phony target to check if any git submodule listed in COMPONENT_SUBMODULES are missing
# or out of date, and exit if so. Components can add paths to this variable.
#
# This only works for components inside IDF_PATH
check-submodules:

# Generate a target to check this submodule
# $(1) - submodule directory, relative to IDF_PATH
define GenerateSubmoduleCheckTarget
check-submodules: $(IDF_PATH)/$(1)/.git
$(IDF_PATH)/$(1)/.git:
	@echo "WARNING: Missing submodule $(1)..."
	[ -d ${IDF_PATH}/.git ] || ( echo "ERROR: esp-idf must be cloned from git to work."; exit 1)
	[ -x $(which git) ] || ( echo "ERROR: Need to run 'git submodule init $(1)' in esp-idf root directory."; exit 1)
	@echo "Attempting 'git submodule update --init $(1)' in esp-idf root directory..."
	cd ${IDF_PATH} && git submodule update --init $(1)

# Parse 'git submodule status' output for out-of-date submodule.
# Status output prefixes status line with '+' if the submodule commit doesn't match
ifneq ("$(shell cd ${IDF_PATH} && git submodule status $(1) | grep '^+')","")
$$(info WARNING: esp-idf git submodule $(1) may be out of date. Run 'git submodule update' in IDF_PATH dir to update.)
endif
endef

# filter/subst in expression ensures all submodule paths begin with $(IDF_PATH), and then strips that prefix
# so the argument is suitable for use with 'git submodule' commands
$(foreach submodule,$(subst $(IDF_PATH)/,,$(filter $(IDF_PATH)/%,$(COMPONENT_SUBMODULES))),$(eval $(call GenerateSubmoduleCheckTarget,$(submodule))))
