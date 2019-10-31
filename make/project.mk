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

.PHONY: build-components menuconfig defconfig all build clean all_binaries check-submodules size size-components size-files size-symbols list-components

MAKECMDGOALS ?= all
all: all_binaries | check_python_dependencies
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
	@echo "make flash - Flash app, bootloader, partition table to a chip"
	@echo "make clean - Remove all build output"
	@echo "make size - Display the static memory footprint of the app"
	@echo "make size-components, size-files - Finer-grained memory footprints"
	@echo "make size-symbols - Per symbol memory footprint. Requires COMPONENT=<component>"
	@echo "make erase_flash - Erase entire flash contents"
	@echo "make erase_otadata - Erase ota_data partition; First bootable partition (factory or OTAx) will be used on next boot."
	@echo "                     This assumes this project's partition table is the one flashed on the device."
	@echo "make monitor - Run idf_monitor tool to monitor serial output from app"
	@echo "make simple_monitor - Monitor serial output on terminal console"
	@echo "make list-components - List all components in the project"
	@echo ""
	@echo "make app - Build just the app"
	@echo "make app-flash - Flash just the app"
	@echo "make app-clean - Clean just the app"
	@echo "make print_flash_cmd - Print the arguments for esptool when flash"
	@echo "make check_python_dependencies - Check that the required python packages are installed"
	@echo ""
	@echo "See also 'make bootloader', 'make bootloader-flash', 'make bootloader-clean', "
	@echo "'make partition_table', etc, etc."

# Non-interactive targets. Mostly, those for which you do not need to build a binary
NON_INTERACTIVE_TARGET += defconfig clean% %clean help list-components print_flash_cmd check_python_dependencies

# dependency checks
ifndef MAKE_RESTARTS
ifeq ("$(filter 4.% 3.81 3.82,$(MAKE_VERSION))","")
$(warning esp-idf build system only supports GNU Make versions 3.81 or newer. You may see unexpected results with other Makes.)
endif

ifdef MSYSTEM
ifneq ("$(MSYSTEM)","MINGW32")
$(warning esp-idf build system only supports MSYS2 in "MINGW32" mode. Consult the ESP-IDF documentation for details.)
endif
endif  # MSYSTEM

endif  # MAKE_RESTARTS

# can't run 'clean' along with any non-clean targets
ifneq ("$(filter clean% %clean,$(MAKECMDGOALS))" ,"")
ifneq ("$(filter-out clean% %clean,$(MAKECMDGOALS))", "")
$(error esp-idf build system doesn't support running 'clean' targets along with any others. Run 'make clean' and then run other targets separately.)
endif
endif

OS ?=

# make IDF_PATH a "real" absolute path
# * works around the case where a shell character is embedded in the environment variable value.
# * changes Windows-style C:/blah/ paths to MSYS style /c/blah
ifeq ("$(OS)","Windows_NT")
# On Windows MSYS2, make wildcard function returns empty string for paths of form /xyz
# where /xyz is a directory inside the MSYS root - so we don't use it.
SANITISED_IDF_PATH:=$(realpath $(IDF_PATH))
else
SANITISED_IDF_PATH:=$(realpath $(wildcard $(IDF_PATH)))
endif

export IDF_PATH := $(SANITISED_IDF_PATH)

ifndef IDF_PATH
$(error IDF_PATH variable is not set to a valid directory.)
endif

ifdef IDF_TARGET
ifneq ($(IDF_TARGET),esp32)
$(error GNU Make based build system only supports esp32 target, but IDF_TARGET is set to $(IDF_TARGET))
endif
else
export IDF_TARGET := esp32
endif


ifneq ("$(IDF_PATH)","$(SANITISED_IDF_PATH)")
# implies IDF_PATH was overriden on make command line.
# Due to the way make manages variables, this is hard to account for
#
# if you see this error, do the shell expansion in the shell ie
# make IDF_PATH=~/blah not make IDF_PATH="~/blah"
$(error If IDF_PATH is overriden on command line, it must be an absolute path with no embedded shell special characters)
endif

ifneq ("$(IDF_PATH)","$(subst :,,$(IDF_PATH))")
$(error IDF_PATH cannot contain colons. If overriding IDF_PATH on Windows, use MSYS Unix-style /c/dir instead of C:/dir)
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
COMMON_MAKEFILES := $(abspath $(IDF_PATH)/make/project.mk $(IDF_PATH)/make/common.mk $(IDF_PATH)/make/version.mk $(IDF_PATH)/make/component_wrapper.mk $(firstword $(MAKEFILE_LIST)))
export COMMON_MAKEFILES

# The directory where we put all objects/libraries/binaries. The project Makefile can
# configure this if needed.
ifndef BUILD_DIR_BASE
BUILD_DIR_BASE := $(PROJECT_PATH)/build
endif
export BUILD_DIR_BASE

# Component directories. These directories are searched for components (either the directory is a component,
# or the directory contains subdirectories which are components.)
# The project Makefile can override these component dirs, or add extras via EXTRA_COMPONENT_DIRS
ifndef COMPONENT_DIRS
EXTRA_COMPONENT_DIRS ?=
COMPONENT_DIRS := $(PROJECT_PATH)/components $(EXTRA_COMPONENT_DIRS) $(IDF_PATH)/components $(PROJECT_PATH)/main
endif
# Make sure that every directory in the list is an absolute path without trailing slash.
# This is necessary to split COMPONENT_DIRS into SINGLE_COMPONENT_DIRS and MULTI_COMPONENT_DIRS below. 
COMPONENT_DIRS := $(foreach cd,$(COMPONENT_DIRS),$(abspath $(cd)))
export COMPONENT_DIRS

ifdef SRCDIRS
$(warning SRCDIRS variable is deprecated. These paths can be added to EXTRA_COMPONENT_DIRS or COMPONENT_DIRS instead.)
COMPONENT_DIRS += $(abspath $(SRCDIRS))
endif

# List of component directories, i.e. directories which contain a component.mk file 
SINGLE_COMPONENT_DIRS := $(abspath $(dir $(dir $(foreach cd,$(COMPONENT_DIRS),\
                             $(wildcard $(cd)/component.mk)))))

# List of components directories, i.e. directories which may contain components 
MULTI_COMPONENT_DIRS := $(filter-out $(SINGLE_COMPONENT_DIRS),$(COMPONENT_DIRS))

# The project Makefile can define a list of components, but if it does not do this
# we just take all available components in the component dirs.
# A component is COMPONENT_DIRS directory, or immediate subdirectory,
# which contains a component.mk file.
#
# Use the "make list-components" target to debug this step.
ifndef COMPONENTS
# Find all component names. The component names are the same as the
# directories they're in, so /bla/components/mycomponent/component.mk -> mycomponent.
# We need to do this for MULTI_COMPONENT_DIRS only, since SINGLE_COMPONENT_DIRS
# are already known to contain component.mk.
COMPONENTS := $(dir $(foreach cd,$(MULTI_COMPONENT_DIRS),$(wildcard $(cd)/*/component.mk))) \
              $(SINGLE_COMPONENT_DIRS)
COMPONENTS := $(sort $(foreach comp,$(COMPONENTS),$(lastword $(subst /, ,$(comp)))))
endif
# After a full manifest of component names is determined, subtract the ones explicitly
# omitted by the project Makefile.
EXCLUDE_COMPONENTS ?=
ifdef EXCLUDE_COMPONENTS
COMPONENTS := $(filter-out $(subst ",,$(EXCLUDE_COMPONENTS)), $(COMPONENTS))
# to keep syntax highlighters happy: "))
endif
export COMPONENTS

# Resolve all of COMPONENTS into absolute paths in COMPONENT_PATHS.
# For each entry in COMPONENT_DIRS:
# - either this is directory with multiple components, in which case check that
#   a subdirectory with component name exists, and it contains a component.mk file.
# - or, this is a directory of a single component, in which case the name of this
#   directory has to match the component name
#
# If a component name exists in multiple COMPONENT_DIRS, we take the first match.
#
# NOTE: These paths must be generated WITHOUT a trailing / so we
# can use $(notdir x) to get the component name.
COMPONENT_PATHS := $(foreach comp,$(COMPONENTS),\
                        $(firstword $(foreach cd,$(COMPONENT_DIRS),\
                            $(if $(findstring $(cd),$(MULTI_COMPONENT_DIRS)),\
                                 $(abspath $(dir $(wildcard $(cd)/$(comp)/component.mk))),)\
                            $(if $(findstring $(cd),$(SINGLE_COMPONENT_DIRS)),\
                                 $(if $(filter $(comp),$(notdir $(cd))),$(cd),),)\
                   )))
export COMPONENT_PATHS

TEST_COMPONENTS ?=
TEST_EXCLUDE_COMPONENTS ?=
TESTS_ALL ?=

# If TESTS_ALL set to 1, set TEST_COMPONENTS_LIST to all components.
# Otherwise, use the list supplied in TEST_COMPONENTS.
ifeq ($(TESTS_ALL),1)
TEST_COMPONENTS_LIST := $(filter-out $(TEST_EXCLUDE_COMPONENTS), $(COMPONENTS))
else
TEST_COMPONENTS_LIST := $(TEST_COMPONENTS)
endif

TEST_COMPONENT_PATHS := $(foreach comp,$(TEST_COMPONENTS_LIST),$(firstword $(foreach dir,$(COMPONENT_DIRS),$(wildcard $(dir)/$(comp)/test))))
TEST_COMPONENT_NAMES := $(foreach comp,$(TEST_COMPONENT_PATHS),$(lastword $(subst /, ,$(dir $(comp))))_test)

# Set default values that were not previously defined
CC ?= gcc
LD ?= ld
AR ?= ar
OBJCOPY ?= objcopy
OBJDUMP ?= objdump
SIZE ?= size

# Set host compiler and binutils
HOSTCC := $(CC)
HOSTLD := $(LD)
HOSTAR := $(AR)
HOSTOBJCOPY := $(OBJCOPY)
HOSTSIZE := $(SIZE)
export HOSTCC HOSTLD HOSTAR HOSTOBJCOPY SIZE

# Set variables common to both project & component (includes config)
include $(IDF_PATH)/make/common.mk

# Notify users when some of the required python packages are not installed
.PHONY: check_python_dependencies
check_python_dependencies:
ifndef IS_BOOTLOADER_BUILD
	$(PYTHON) $(IDF_PATH)/tools/check_python_dependencies.py
endif

# include the config generation targets (dependency: COMPONENT_PATHS)
#
# (bootloader build doesn't need this, config is exported from top-level)
ifndef IS_BOOTLOADER_BUILD
include $(IDF_PATH)/make/project_config.mk
endif

#####################################################################
# If SDKCONFIG_MAKEFILE hasn't been generated yet (detected if no
# CONFIG_IDF_TARGET), stop the Makefile pass now to allow config to
# be created. make will build SDKCONFIG_MAKEFILE and restart,
# reevaluating everything from the top.
#
# This is important so config is present when the
# component_project_vars.mk files are generated.
#
# (After both files exist, if SDKCONFIG_MAKEFILE is updated then the
# normal dependency relationship will trigger a regeneration of
# component_project_vars.mk)
#
#####################################################################
ifndef CONFIG_IDF_TARGET
ifdef IS_BOOTLOADER_BUILD  # we expect config to always have been expanded by top level project
$(error "Internal error: config has not been passed correctly to bootloader subproject")
endif
ifdef MAKE_RESTARTS
$(warning "Config was not evaluated after the first pass of 'make'")
endif
else  # CONFIG_IDF_TARGET
#####################################################################
# Config is valid, can include rest of the Project Makefile
#####################################################################


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
COMPONENT_LIBRARIES :=
COMPONENT_LDFRAGMENTS :=

# COMPONENT_PROJECT_VARS is the list of component_project_vars.mk generated makefiles
# for each component.
#
# Including $(COMPONENT_PROJECT_VARS) builds the COMPONENT_INCLUDES,
# COMPONENT_LDFLAGS variables and also targets for any inter-component
# dependencies.
#
# See the component_project_vars.mk target in component_wrapper.mk
COMPONENT_PROJECT_VARS := $(addsuffix /component_project_vars.mk,$(notdir $(COMPONENT_PATHS) ) $(TEST_COMPONENT_NAMES))
COMPONENT_PROJECT_VARS := $(addprefix $(BUILD_DIR_BASE)/,$(COMPONENT_PROJECT_VARS))
# this line is -include instead of include to prevent a spurious error message on make 3.81
-include $(COMPONENT_PROJECT_VARS)

# Also add top-level project include path, for top-level includes
COMPONENT_INCLUDES += $(abspath $(BUILD_DIR_BASE)/include/)

export COMPONENT_INCLUDES

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
ifdef CONFIG_APP_BUILD_GENERATE_BINARIES
	@echo "To flash all build output, run 'make flash' or:"
endif
endif
ifdef CONFIG_APP_BUILD_GENERATE_BINARIES
	@echo $(ESPTOOLPY_WRITE_FLASH) $(ESPTOOL_ALL_FLASH_ARGS)
else
	@echo "Binary is not available for flashing"
endif


# If we have `version.txt` then prefer that for extracting IDF version
ifeq ("$(wildcard ${IDF_PATH}/version.txt)","")
IDF_VER_T := $(shell cd ${IDF_PATH} && git describe --always --tags --dirty)
else
IDF_VER_T := $(shell cat ${IDF_PATH}/version.txt)
endif
IDF_VER := $(shell echo "$(IDF_VER_T)"  | cut -c 1-31)

# Set default LDFLAGS
EXTRA_LDFLAGS ?=
LDFLAGS ?= -nostdlib \
	-u call_user_start_cpu0	\
	$(EXTRA_LDFLAGS) \
	-Wl,--gc-sections	\
	-Wl,-static	\
	-Wl,--start-group	\
	$(COMPONENT_LDFLAGS) \
	-lgcc \
	-lstdc++ \
	-lgcov \
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
CPPFLAGS ?=
EXTRA_CPPFLAGS ?=
CPPFLAGS := -DESP_PLATFORM -D IDF_VER=\"$(IDF_VER)\" -MMD -MP $(CPPFLAGS) $(EXTRA_CPPFLAGS)
PROJECT_VER ?=
export IDF_VER
export PROJECT_NAME
export PROJECT_VER

# Warnings-related flags relevant both for C and C++
COMMON_WARNING_FLAGS = -Wall -Werror=all \
	-Wno-error=unused-function \
	-Wno-error=unused-but-set-variable \
	-Wno-error=unused-variable \
	-Wno-error=deprecated-declarations \
	-Wextra \
	-Wno-unused-parameter -Wno-sign-compare

ifdef CONFIG_COMPILER_DISABLE_GCC8_WARNINGS
COMMON_WARNING_FLAGS += -Wno-parentheses \
	-Wno-sizeof-pointer-memaccess \
	-Wno-clobbered \
	-Wno-format-overflow \
	-Wno-stringop-truncation \
	-Wno-misleading-indentation \
	-Wno-cast-function-type \
	-Wno-implicit-fallthrough \
	-Wno-unused-const-variable \
	-Wno-switch-unreachable \
	-Wno-format-truncation \
	-Wno-memset-elt-size \
	-Wno-int-in-bool-context
endif

ifdef CONFIG_COMPILER_WARN_WRITE_STRINGS
COMMON_WARNING_FLAGS += -Wwrite-strings
endif #CONFIG_COMPILER_WARN_WRITE_STRINGS

# Flags which control code generation and dependency generation, both for C and C++
COMMON_FLAGS = \
	-Wno-frame-address \
	-ffunction-sections -fdata-sections \
	-fstrict-volatile-bitfields \
	-mlongcalls \
	-nostdlib

ifndef IS_BOOTLOADER_BUILD
# stack protection (only one option can be selected in menuconfig)
ifdef CONFIG_COMPILER_STACK_CHECK_MODE_NORM
COMMON_FLAGS += -fstack-protector
endif
ifdef CONFIG_COMPILER_STACK_CHECK_MODE_STRONG
COMMON_FLAGS += -fstack-protector-strong
endif
ifdef CONFIG_COMPILER_STACK_CHECK_MODE_ALL
COMMON_FLAGS += -fstack-protector-all
endif
endif

# Optimization flags are set based on menuconfig choice
ifdef CONFIG_COMPILER_OPTIMIZATION_SIZE
OPTIMIZATION_FLAGS = -Os -freorder-blocks
endif

ifdef CONFIG_COMPILER_OPTIMIZATION_DEFAULT
OPTIMIZATION_FLAGS = -Og
endif

ifdef CONFIG_COMPILER_OPTIMIZATION_NONE
OPTIMIZATION_FLAGS = -O0
endif

ifdef CONFIG_COMPILER_OPTIMIZATION_PERF
OPTIMIZATION_FLAGS = -O2
endif

ifdef CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE
CPPFLAGS += -DNDEBUG
endif

# IDF uses some GNU extension from libc
CPPFLAGS += -D_GNU_SOURCE

# Enable generation of debugging symbols
# (we generate even in Release mode, as this has no impact on final binary size.)
DEBUG_FLAGS ?= -ggdb

# List of flags to pass to C compiler
# If any flags are defined in application Makefile, add them at the end.
CFLAGS ?=
EXTRA_CFLAGS ?=
CFLAGS := $(strip \
	-std=gnu99 \
	$(OPTIMIZATION_FLAGS) $(DEBUG_FLAGS) \
	$(COMMON_FLAGS) \
	$(COMMON_WARNING_FLAGS) -Wno-old-style-declaration \
	$(CFLAGS) \
	$(EXTRA_CFLAGS))

# List of flags to pass to C++ compiler
# If any flags are defined in application Makefile, add them at the end.
CXXFLAGS ?=
EXTRA_CXXFLAGS ?=
CXXFLAGS := $(strip \
	-std=gnu++11 \
	$(OPTIMIZATION_FLAGS) $(DEBUG_FLAGS) \
	$(COMMON_FLAGS) \
	$(COMMON_WARNING_FLAGS) \
	$(CXXFLAGS) \
	$(EXTRA_CXXFLAGS))

ifdef CONFIG_COMPILER_CXX_EXCEPTIONS
CXXFLAGS += -fexceptions
else
CXXFLAGS += -fno-exceptions
endif

ifdef CONFIG_COMPILER_CXX_RTTI
CXXFLAGS += -frtti
else
CXXFLAGS += -fno-rtti
LDFLAGS += -fno-rtti
endif

ARFLAGS := cru

export CFLAGS CPPFLAGS CXXFLAGS ARFLAGS

# Set target compiler. Defaults to whatever the user has
# configured as prefix + ye olde gcc commands
CC := $(call dequote,$(CONFIG_SDK_TOOLPREFIX))gcc
CXX := $(call dequote,$(CONFIG_SDK_TOOLPREFIX))c++
LD := $(call dequote,$(CONFIG_SDK_TOOLPREFIX))ld
AR := $(call dequote,$(CONFIG_SDK_TOOLPREFIX))ar
OBJCOPY := $(call dequote,$(CONFIG_SDK_TOOLPREFIX))objcopy
OBJDUMP := $(call dequote,$(CONFIG_SDK_TOOLPREFIX))objdump
SIZE := $(call dequote,$(CONFIG_SDK_TOOLPREFIX))size
export CC CXX LD AR OBJCOPY OBJDUMP SIZE

COMPILER_VERSION_STR := $(shell $(CC) -dumpversion)
COMPILER_VERSION_NUM := $(subst .,,$(COMPILER_VERSION_STR))
GCC_NOT_5_2_0 := $(shell expr $(COMPILER_VERSION_STR) != "5.2.0")
export COMPILER_VERSION_STR COMPILER_VERSION_NUM GCC_NOT_5_2_0

CPPFLAGS += -DGCC_NOT_5_2_0=$(GCC_NOT_5_2_0)
export CPPFLAGS


# the app is the main executable built by the project
APP_ELF:=$(BUILD_DIR_BASE)/$(PROJECT_NAME).elf
APP_MAP:=$(APP_ELF:.elf=.map)
APP_BIN:=$(APP_ELF:.elf=.bin)

# include linker script generation utils makefile
include $(IDF_PATH)/make/ldgen.mk

$(eval $(call ldgen_create_commands))

# Include any Makefile.projbuild file letting components add
# configuration at the project level
define includeProjBuildMakefile
ifeq ("$(V)","1")
$$(info including $(1)/Makefile.projbuild...)
endif
COMPONENT_PATH := $(1)
include $(1)/Makefile.projbuild
endef
$(foreach componentpath,$(COMPONENT_PATHS), \
	$(if $(wildcard $(componentpath)/Makefile.projbuild), \
		$(eval $(call includeProjBuildMakefile,$(componentpath)))))

# ELF depends on the library archive files for COMPONENT_LIBRARIES
# the rules to build these are emitted as part of GenerateComponentTarget below
#
# also depends on additional dependencies (linker scripts & binary libraries)
# stored in COMPONENT_LINKER_DEPS, built via component.mk files' COMPONENT_ADD_LINKER_DEPS variable
COMPONENT_LINKER_DEPS ?=
$(APP_ELF): $(foreach libcomp,$(COMPONENT_LIBRARIES),$(BUILD_DIR_BASE)/$(libcomp)/lib$(libcomp).a) $(COMPONENT_LINKER_DEPS) $(COMPONENT_PROJECT_VARS)
	$(summary) LD $(patsubst $(PWD)/%,%,$@)
	$(CC) $(LDFLAGS) -o $@ -Wl,-Map=$(APP_MAP)

app: $(APP_BIN) partition_table_get_info
ifeq ("$(CONFIG_APP_BUILD_GENERATE_BINARIES)","y")
ifeq ("$(CONFIG_SECURE_BOOT_ENABLED)$(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)","y") # secure boot enabled, but remote sign app image
	@echo "App built but not signed. Signing step via espsecure.py:"
	@echo "espsecure.py sign_data --keyfile KEYFILE $(APP_BIN)"
	@echo "Then flash app command is:"
	@echo $(ESPTOOLPY_WRITE_FLASH) $(APP_OFFSET) $(APP_BIN)
else
	@echo "App built. Default flash app command is:"
	@echo $(ESPTOOLPY_WRITE_FLASH) $(APP_OFFSET) $(APP_BIN)
endif
else
	@echo "Application in not built and cannot be flashed."
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
.PHONY: component-$(2)-build component-$(2)-clean

component-$(2)-build: check-submodules $(call prereq_if_explicit, component-$(2)-clean) | $(BUILD_DIR_BASE)/$(2)
	$(call ComponentMake,$(1),$(2)) build

component-$(2)-clean: | $(BUILD_DIR_BASE)/$(2) $(BUILD_DIR_BASE)/$(2)/component_project_vars.mk
	$(call ComponentMake,$(1),$(2)) clean

$(BUILD_DIR_BASE)/$(2):
	@mkdir -p $(BUILD_DIR_BASE)/$(2)

# tell make it can build any component's library by invoking the -build target
# (this target exists for all components even ones which don't build libraries, but it's
# only invoked for the targets whose libraries appear in COMPONENT_LIBRARIES and hence the
# APP_ELF dependencies.)
$(BUILD_DIR_BASE)/$(2)/lib$(2).a: component-$(2)-build
	$(details) "Target '$$^' responsible for '$$@'" # echo which build target built this file

# add a target to generate the component_project_vars.mk files that
# are used to inject variables into project make pass (see matching
# component_project_vars.mk target in component_wrapper.mk).
#
# If any component_project_vars.mk file is out of date, the make
# process will call this target to rebuild it and then restart.
#
$(BUILD_DIR_BASE)/$(2)/component_project_vars.mk: $(1)/component.mk $(COMMON_MAKEFILES) $(SDKCONFIG_MAKEFILE) | $(BUILD_DIR_BASE)/$(2)
	$(call ComponentMake,$(1),$(2)) component_project_vars.mk
endef

$(foreach component,$(COMPONENT_PATHS),$(eval $(call GenerateComponentTargets,$(component),$(notdir $(component)))))
$(foreach component,$(TEST_COMPONENT_PATHS),$(eval $(call GenerateComponentTargets,$(component),$(lastword $(subst /, ,$(dir $(component))))_test)))

app-clean: $(addprefix component-,$(addsuffix -clean,$(notdir $(COMPONENT_PATHS))))
	$(summary) RM $(APP_ELF)
	rm -f $(APP_ELF) $(APP_BIN) $(APP_MAP)

size: $(APP_ELF) | check_python_dependencies
	$(PYTHON) $(IDF_PATH)/tools/idf_size.py $(APP_MAP)

size-files: $(APP_ELF) | check_python_dependencies
	$(PYTHON) $(IDF_PATH)/tools/idf_size.py --files $(APP_MAP)

size-components: $(APP_ELF) | check_python_dependencies
	$(PYTHON) $(IDF_PATH)/tools/idf_size.py --archives $(APP_MAP)

size-symbols: $(APP_ELF) | check_python_dependencies
ifndef COMPONENT
	$(error "ERROR: Please enter the component to look symbols for, e.g. COMPONENT=heap")
else
	$(PYTHON) $(IDF_PATH)/tools/idf_size.py --archive_details lib$(COMPONENT).a $(APP_MAP)
endif

# NB: this ordering is deliberate (app-clean & bootloader-clean before
# _config-clean), so config remains valid during all component clean
# targets
config-clean: app-clean bootloader-clean
clean: app-clean bootloader-clean config-clean ldgen-clean

# phony target to check if any git submodule listed in COMPONENT_SUBMODULES are missing
# or out of date, and exit if so. Components can add paths to this variable.
#
# This only works for components inside IDF_PATH
check-submodules:
# Check if .gitmodules exists, otherwise skip submodule check, assuming flattened structure
ifneq ("$(wildcard ${IDF_PATH}/.gitmodules)","")

# Dump the git status for the whole working copy once, then grep it for each submodule. This saves a lot of time on Windows.
GIT_STATUS := $(shell cd ${IDF_PATH} && git status --porcelain --ignore-submodules=dirty)

# Generate a target to check this submodule
# $(1) - submodule directory, relative to IDF_PATH
define GenerateSubmoduleCheckTarget
check-submodules: $(IDF_PATH)/$(1)/.git
$(IDF_PATH)/$(1)/.git:
	@echo "WARNING: Missing submodule $(1)..."
	[ -e ${IDF_PATH}/.git ] || { echo "ERROR: esp-idf must be cloned from git to work."; exit 1; }
	[ -x "$(shell which git)" ] || { echo "ERROR: Need to run 'git submodule init $(1)' in esp-idf root directory."; exit 1; }
	@echo "Attempting 'git submodule update --init $(1)' in esp-idf root directory..."
	cd ${IDF_PATH} && git submodule update --init $(1)

# Parse 'git status' output to check if the submodule commit is different to expected
ifneq ("$(filter $(1),$(GIT_STATUS))","")
$$(info WARNING: esp-idf git submodule $(1) may be out of date. Run 'git submodule update' in IDF_PATH dir to update.)
endif
endef

# filter/subst in expression ensures all submodule paths begin with $(IDF_PATH), and then strips that prefix
# so the argument is suitable for use with 'git submodule' commands
$(foreach submodule,$(subst $(IDF_PATH)/,,$(filter $(IDF_PATH)/%,$(COMPONENT_SUBMODULES))),$(eval $(call GenerateSubmoduleCheckTarget,$(submodule))))
endif # End check for .gitmodules existence


# PHONY target to list components in the build and their paths
list-components:
	$(info $(call dequote,$(SEPARATOR)))
	$(info COMPONENT_DIRS (components searched for here))
	$(foreach cd,$(COMPONENT_DIRS),$(info $(cd)))
	$(info $(call dequote,$(SEPARATOR)))
	$(info TEST_COMPONENTS (list of test component names))
	$(info $(TEST_COMPONENTS_LIST))
	$(info $(call dequote,$(SEPARATOR)))
	$(info TEST_EXCLUDE_COMPONENTS (list of test excluded names))
	$(info $(if $(EXCLUDE_COMPONENTS) || $(TEST_EXCLUDE_COMPONENTS),$(EXCLUDE_COMPONENTS) $(TEST_EXCLUDE_COMPONENTS),(none provided)))
	$(info $(call dequote,$(SEPARATOR)))
	$(info COMPONENT_PATHS (paths to all components):)
	$(foreach cp,$(COMPONENT_PATHS),$(info $(cp)))

# print flash command, so users can dump this to config files and download somewhere without idf
print_flash_cmd: partition_table_get_info blank_ota_data
	echo $(ESPTOOL_WRITE_FLASH_OPTIONS) $(ESPTOOL_ALL_FLASH_ARGS) | sed -e 's:'$(PWD)/build/'::g'

# Check toolchain version using the output of xtensa-esp32-elf-gcc --version command.
# The output normally looks as follows
#     xtensa-esp32-elf-gcc (crosstool-NG crosstool-ng-1.22.0-80-g6c4433a) 5.2.0
# The part in brackets is extracted into TOOLCHAIN_COMMIT_DESC variable
ifdef CONFIG_SDK_TOOLPREFIX
ifndef MAKE_RESTARTS

TOOLCHAIN_HEADER := $(shell $(CC) --version | head -1)
TOOLCHAIN_PATH := $(shell which $(CC))
TOOLCHAIN_COMMIT_DESC := $(shell $(CC) --version | sed -E -n 's|.*\(crosstool-NG (.*)\).*|\1|gp')
TOOLCHAIN_GCC_VER := $(COMPILER_VERSION_STR)

# Officially supported version(s)
include $(IDF_PATH)/tools/toolchain_versions.mk

ifndef IS_BOOTLOADER_BUILD
$(info Toolchain path: $(TOOLCHAIN_PATH))
endif

ifdef TOOLCHAIN_COMMIT_DESC
ifeq (,$(findstring $(SUPPORTED_TOOLCHAIN_COMMIT_DESC),$(TOOLCHAIN_COMMIT_DESC)))
$(info WARNING: Toolchain version is not supported: $(TOOLCHAIN_COMMIT_DESC))
$(info Expected to see version: $(SUPPORTED_TOOLCHAIN_COMMIT_DESC))
$(info Please check ESP-IDF setup instructions and update the toolchain, or proceed at your own risk.)
else
ifndef IS_BOOTLOADER_BUILD
$(info Toolchain version: $(TOOLCHAIN_COMMIT_DESC))
endif
endif
ifeq (,$(findstring $(TOOLCHAIN_GCC_VER), $(SUPPORTED_TOOLCHAIN_GCC_VERSIONS)))
$(info WARNING: Compiler version is not supported: $(TOOLCHAIN_GCC_VER))
$(info Expected to see version(s): $(SUPPORTED_TOOLCHAIN_GCC_VERSIONS))
$(info Please check ESP-IDF setup instructions and update the toolchain, or proceed at your own risk.)
else
ifndef IS_BOOTLOADER_BUILD
$(info Compiler version: $(TOOLCHAIN_GCC_VER))
endif
endif
else
$(info WARNING: Failed to find Xtensa toolchain, may need to alter PATH or set one in the configuration menu)
endif # TOOLCHAIN_COMMIT_DESC

endif #MAKE_RESTARTS
endif #CONFIG_SDK_TOOLPREFIX

#####################################################################
endif #CONFIG_IDF_TARGET

