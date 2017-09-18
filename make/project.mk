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

.PHONY: build-components menuconfig defconfig all build clean all_binaries check-submodules size size-components size-files list-components
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
	@echo "make flash - Flash app, bootloader, partition table to a chip"
	@echo "make clean - Remove all build output"
	@echo "make size - Display the static memory footprint of the app"
	@echo "make size-components, size-files - Finer-grained memory footprints"
	@echo "make erase_flash - Erase entire flash contents"
	@echo "make monitor - Run idf_monitor tool to monitor serial output from app"
	@echo "make simple_monitor - Monitor serial output on terminal console"
	@echo "make list-components - List all components in the project"
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
$(warning esp-idf build system only supports GNU Make versions 3.81 or newer. You may see unexpected results with other Makes.)
endif
endif

# can't run 'clean' along with any non-clean targets
ifneq ("$(filter clean% %clean,$(MAKECMDGOALS))" ,"")
ifneq ("$(filter-out clean% %clean,$(MAKECMDGOALS))", "")
$(error esp-idf build system doesn't support running 'clean' targets along with any others. Run 'make clean' and then run other targets separately.)
endif
endif

OS ?=

# make IDF_PATH a "real" absolute path
# * works around the case where a shell character is embedded in the environment variable value.
# * changes Windows-style C:/blah/ paths to MSYS/Cygwin style /c/blah
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

ifneq ("$(IDF_PATH)","$(SANITISED_IDF_PATH)")
# implies IDF_PATH was overriden on make command line.
# Due to the way make manages variables, this is hard to account for
#
# if you see this error, do the shell expansion in the shell ie
# make IDF_PATH=~/blah not make IDF_PATH="~/blah"
$(error If IDF_PATH is overriden on command line, it must be an absolute path with no embedded shell special characters)
endif

ifneq ("$(IDF_PATH)","$(subst :,,$(IDF_PATH))")
$(error IDF_PATH cannot contain colons. If overriding IDF_PATH on Windows, use Cygwin-style /c/dir instead of C:/dir)
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
COMMON_MAKEFILES := $(abspath $(IDF_PATH)/make/project.mk $(IDF_PATH)/make/common.mk $(IDF_PATH)/make/component_wrapper.mk $(firstword $(MAKEFILE_LIST)))
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
export COMPONENT_DIRS

ifdef SRCDIRS
$(warning SRCDIRS variable is deprecated. These paths can be added to EXTRA_COMPONENT_DIRS or COMPONENT_DIRS instead.)
COMPONENT_DIRS += $(abspath $(SRCDIRS))
endif

# The project Makefile can define a list of components, but if it does not do this we just take all available components
# in the component dirs. A component is COMPONENT_DIRS directory, or immediate subdirectory,
# which contains a component.mk file.
#
# Use the "make list-components" target to debug this step.
ifndef COMPONENTS
# Find all component names. The component names are the same as the
# directories they're in, so /bla/components/mycomponent/component.mk -> mycomponent.
COMPONENTS := $(dir $(foreach cd,$(COMPONENT_DIRS),                           \
					$(wildcard $(cd)/*/component.mk) $(wildcard $(cd)/component.mk) \
				))
COMPONENTS := $(sort $(foreach comp,$(COMPONENTS),$(lastword $(subst /, ,$(comp)))))
endif
export COMPONENTS

# Resolve all of COMPONENTS into absolute paths in COMPONENT_PATHS.
#
# If a component name exists in multiple COMPONENT_DIRS, we take the first match.
#
# NOTE: These paths must be generated WITHOUT a trailing / so we
# can use $(notdir x) to get the component name.
COMPONENT_PATHS := $(foreach comp,$(COMPONENTS),$(firstword $(foreach cd,$(COMPONENT_DIRS),$(wildcard $(dir $(cd))$(comp) $(cd)/$(comp)))))

TEST_COMPONENTS ?=
TESTS_ALL ?=

# If TESTS_ALL set to 1, set TEST_COMPONENTS_LIST to all components.
# Otherwise, use the list supplied in TEST_COMPONENTS.
ifeq ($(TESTS_ALL),1)
TEST_COMPONENTS_LIST := $(COMPONENTS)
else
TEST_COMPONENTS_LIST := $(TEST_COMPONENTS)
endif

TEST_COMPONENT_PATHS := $(foreach comp,$(TEST_COMPONENTS_LIST),$(firstword $(foreach dir,$(COMPONENT_DIRS),$(wildcard $(dir)/$(comp)/test))))
TEST_COMPONENT_NAMES := $(foreach comp,$(TEST_COMPONENT_PATHS),$(lastword $(subst /, ,$(dir $(comp))))_test)

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


IDF_VER := $(shell cd ${IDF_PATH} && git describe --always --tags --dirty)

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
ifdef CONFIG_OPTIMIZATION_LEVEL_RELEASE
OPTIMIZATION_FLAGS = -Os
else
OPTIMIZATION_FLAGS = -Og
endif

ifdef CONFIG_OPTIMIZATION_ASSERTIONS_DISABLED
CPPFLAGS += -DNDEBUG
endif

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
	-fno-rtti \
	$(OPTIMIZATION_FLAGS) $(DEBUG_FLAGS) \
	$(COMMON_FLAGS) \
	$(COMMON_WARNING_FLAGS) \
	$(CXXFLAGS) \
	$(EXTRA_CXXFLAGS))

export CFLAGS CPPFLAGS CXXFLAGS

# Set host compiler and binutils
HOSTCC := $(CC)
HOSTLD := $(LD)
HOSTAR := $(AR)
ifdef OBJCOPY
HOSTOBJCOPY := $(OBJCOPY)
else
HOSTOBJCOPY := objcopy
endif
ifdef SIZE
HOSTSIZE := $(SIZE)
else
HOSTSIZE := size
endif
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
$(if $(V),$(info including $(1)/Makefile.projbuild...))
COMPONENT_PATH := $(1)
include $(1)/Makefile.projbuild
endef
$(foreach componentpath,$(COMPONENT_PATHS), \
	$(if $(wildcard $(componentpath)/Makefile.projbuild), \
		$(eval $(call includeProjBuildMakefile,$(componentpath)))))

# once we know component paths, we can include the config generation targets
#
# (bootloader build doesn't need this, config is exported from top-level)
ifndef IS_BOOTLOADER_BUILD
include $(IDF_PATH)/make/project_config.mk
endif

# ELF depends on the library archive files for COMPONENT_LIBRARIES
# the rules to build these are emitted as part of GenerateComponentTarget below
#
# also depends on additional dependencies (linker scripts & binary libraries)
# stored in COMPONENT_LINKER_DEPS, built via component.mk files' COMPONENT_ADD_LINKER_DEPS variable
COMPONENT_LINKER_DEPS ?=
$(APP_ELF): $(foreach libcomp,$(COMPONENT_LIBRARIES),$(BUILD_DIR_BASE)/$(libcomp)/lib$(libcomp).a) $(COMPONENT_LINKER_DEPS) $(COMPONENT_PROJECT_VARS)
	$(summary) LD $(patsubst $(PWD)/%,%,$@)
	$(CC) $(LDFLAGS) -o $@ -Wl,-Map=$(APP_MAP)

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

size: $(APP_ELF)
	$(PYTHON) $(IDF_PATH)/tools/idf_size.py $(APP_MAP)

size-files: $(APP_ELF)
	$(PYTHON) $(IDF_PATH)/tools/idf_size.py --files $(APP_MAP)

size-components: $(APP_ELF)
	$(PYTHON) $(IDF_PATH)/tools/idf_size.py --archives $(APP_MAP)

# NB: this ordering is deliberate (app-clean & bootloader-clean before
# _config-clean), so config remains valid during all component clean
# targets
config-clean: app-clean bootloader-clean
clean: app-clean bootloader-clean config-clean

# phony target to check if any git submodule listed in COMPONENT_SUBMODULES are missing
# or out of date, and exit if so. Components can add paths to this variable.
#
# This only works for components inside IDF_PATH
check-submodules:

# Dump the git status for the whole working copy once, then grep it for each submodule. This saves a lot of time on Windows.
GIT_STATUS := $(shell cd ${IDF_PATH} && git status --porcelain --ignore-submodules=dirty)

# Generate a target to check this submodule
# $(1) - submodule directory, relative to IDF_PATH
define GenerateSubmoduleCheckTarget
check-submodules: $(IDF_PATH)/$(1)/.git
$(IDF_PATH)/$(1)/.git:
	@echo "WARNING: Missing submodule $(1)..."
	[ -e ${IDF_PATH}/.git ] || ( echo "ERROR: esp-idf must be cloned from git to work."; exit 1)
	[ -x $$(which git) ] || ( echo "ERROR: Need to run 'git submodule init $(1)' in esp-idf root directory."; exit 1)
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


# PHONY target to list components in the build and their paths
list-components:
	$(info $(call dequote,$(SEPARATOR)))
	$(info COMPONENT_DIRS (components searched for here))
	$(foreach cd,$(COMPONENT_DIRS),$(info $(cd)))
	$(info $(call dequote,$(SEPARATOR)))
	$(info COMPONENTS (list of component names))
	$(info $(COMPONENTS))
	$(info $(call dequote,$(SEPARATOR)))
	$(info COMPONENT_PATHS (paths to all components):)
	$(foreach cp,$(COMPONENT_PATHS),$(info $(cp)))

# Check toolchain version using the output of xtensa-esp32-elf-gcc --version command.
# The output normally looks as follows
#     xtensa-esp32-elf-gcc (crosstool-NG crosstool-ng-1.22.0-59-ga194053) 4.8.5
# The part in brackets is extracted into TOOLCHAIN_COMMIT_DESC variable,
# the part after the brackets is extracted into TOOLCHAIN_GCC_VER.
ifdef CONFIG_TOOLPREFIX
ifndef MAKE_RESTARTS
TOOLCHAIN_COMMIT_DESC := $(shell $(CC) --version | sed -E -n 's|.*crosstool-ng-([0-9]+).([0-9]+).([0-9]+)-([0-9]+)-g([0-9a-f]{7}).*|\1.\2.\3-\4-g\5|gp')
TOOLCHAIN_GCC_VER := $(shell $(CC) --version | sed -E -n 's|xtensa-esp32-elf-gcc.*\ \(.*\)\ (.*)|\1|gp')

# Officially supported version(s)
SUPPORTED_TOOLCHAIN_COMMIT_DESC := 1.22.0-73-ge28a011
SUPPORTED_TOOLCHAIN_GCC_VERSIONS := 5.2.0

ifdef TOOLCHAIN_COMMIT_DESC
ifneq ($(TOOLCHAIN_COMMIT_DESC), $(SUPPORTED_TOOLCHAIN_COMMIT_DESC))
$(info WARNING: Toolchain version is not supported: $(TOOLCHAIN_COMMIT_DESC))
$(info Expected to see version: $(SUPPORTED_TOOLCHAIN_COMMIT_DESC))
$(info Please check ESP-IDF setup instructions and update the toolchain, or proceed at your own risk.)
endif
ifeq (,$(findstring $(TOOLCHAIN_GCC_VER), $(SUPPORTED_TOOLCHAIN_GCC_VERSIONS)))
$(info WARNING: Compiler version is not supported: $(TOOLCHAIN_GCC_VER))
$(info Expected to see version(s): $(SUPPORTED_TOOLCHAIN_GCC_VERSIONS))
$(info Please check ESP-IDF setup instructions and update the toolchain, or proceed at your own risk.)
endif
else
$(info WARNING: Failed to find Xtensa toolchain, may need to alter PATH or set one in the configuration menu)
endif # TOOLCHAIN_COMMIT_DESC

endif #MAKE_RESTARTS
endif #CONFIG_TOOLPREFIX
