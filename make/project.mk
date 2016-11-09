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

.PHONY: build-components menuconfig defconfig all build clean all_binaries
all: all_binaries # other components will add dependencies to 'all_binaries'
	@echo "To flash all build output, run 'make flash' or:"
	@echo $(ESPTOOLPY_WRITE_FLASH) $(ESPTOOL_ALL_FLASH_ARGS)

# (the reason all_binaries is used instead of 'all' is so that the flash target
# can build everything without triggering the per-component "to flash..."
# output targets.)

help:
	@echo "Welcome to Espressif IDF build system. Some useful make targets:"
	@echo ""
	@echo "make menuconfig - Configure IDF project"
	@echo "make defconfig - Set defaults for all new configuration options"
	@echo ""
	@echo "make all - Build app, bootloader, partition table"
	@echo "make flash - Flash all components to a fresh chip"
	@echo "make clean - Remove all build output"
	@echo ""
	@echo "make app - Build just the app"
	@echo "make app-flash - Flash just the app"
	@echo "make app-clean - Clean just the app"
	@echo ""
	@echo "See also 'make bootloader', 'make bootloader-flash', 'make bootloader-clean', "
	@echo "'make partition_table', etc, etc."

# disable built-in make rules, makes debugging saner
MAKEFLAGS_OLD := $(MAKEFLAGS)
MAKEFLAGS +=-rR

# Figure out PROJECT_PATH if not set
ifeq ("$(PROJECT_PATH)","")
#The path to the project: we assume the Makefile including this file resides
#in the root of that directory.
PROJECT_PATH := $(abspath $(dir $(firstword $(MAKEFILE_LIST))))
export PROJECT_PATH
endif

COMMON_MAKEFILES := $(abspath $(IDF_PATH)/make/project.mk $(IDF_PATH)/make/common.mk $(IDF_PATH)/make/component_common.mk)
export COMMON_MAKEFILES

#The directory where we put all objects/libraries/binaries. The project Makefile can
#configure this if needed.
BUILD_DIR_BASE ?= $(PROJECT_PATH)/build
export BUILD_DIR_BASE

ifndef IS_BOOTLOADER_BUILD
# Include project config file, if it exists.
# (bootloader build doesn't need this, config is exported from top-level)
#
# (Note that we only rebuild auto.conf automatically for some targets,
# see project_config.mk for details.)
#
SDKCONFIG_MAKEFILE := $(BUILD_DIR_BASE)/include/config/auto.conf
-include $(SDKCONFIG_MAKEFILE)
export $(filter CONFIG_%,$(.VARIABLES))
endif

#Component directories. These directories are searched for components.
#The project Makefile can override these component dirs, or define extra component directories.
COMPONENT_DIRS ?= $(PROJECT_PATH)/components $(EXTRA_COMPONENT_DIRS) $(IDF_PATH)/components
export COMPONENT_DIRS

#The project Makefile can define a list of components, but if it does not do this we just take
#all available components in the component dirs.
ifeq ("$(COMPONENTS)","")
#Find all component names. The component names are the same as the
#directories they're in, so /bla/components/mycomponent/ -> mycomponent. We later use
#the COMPONENT_DIRS bit to find back the component path.
COMPONENTS := $(foreach dir,$(COMPONENT_DIRS),$(wildcard $(dir)/*))
COMPONENTS := $(sort $(foreach comp,$(COMPONENTS),$(lastword $(subst /, ,$(comp)))))
endif
export COMPONENTS

#Sources default to only "main"
SRCDIRS ?= main

#Here, we resolve and add all the components and source paths into absolute paths.
#If a component exists in multiple COMPONENT_DIRS, we take the first match.
#WARNING: These directories paths must be generated WITHOUT a trailing / so we
#can use $(notdir x) to get the component name.
COMPONENT_PATHS := $(foreach comp,$(COMPONENTS),$(firstword $(foreach dir,$(COMPONENT_DIRS),$(wildcard $(dir)/$(comp)))))
COMPONENT_PATHS += $(abspath $(SRCDIRS))

#A component is buildable if it has a component.mk makefile; we assume that a
# 'make -C $(component dir) -f component.mk build' results in a lib$(componentname).a
COMPONENT_PATHS_BUILDABLE := $(foreach cp,$(COMPONENT_PATHS),$(if $(wildcard $(cp)/component.mk),$(cp)))

# Assemble global list of include dirs (COMPONENT_INCLUDES), and
# LDFLAGS args (COMPONENT_LDFLAGS) supplied by each component.
COMPONENT_INCLUDES :=
COMPONENT_LDFLAGS :=

# include paths for generated "component project variables" targets with
# COMPONENT_INCLUDES, COMPONENT_LDFLAGS & dependency targets
#
# See component_project_vars.mk target in component_common.mk
COMPONENT_PROJECT_VARS := $(addsuffix /component_project_vars.mk,$(notdir $(COMPONENT_PATHS_BUILDABLE)))
COMPONENT_PROJECT_VARS := $(addprefix $(BUILD_DIR_BASE)/,$(COMPONENT_PROJECT_VARS))
include $(COMPONENT_PROJECT_VARS)

#Also add project include path, for top-level includes
COMPONENT_INCLUDES += $(abspath $(BUILD_DIR_BASE)/include/)

export COMPONENT_INCLUDES
export COMPONENT_LDFLAGS

#Make sure submakes can also use this.
export PROJECT_PATH

#Include functionality common to both project & component
-include $(IDF_PATH)/make/common.mk

# Set default LDFLAGS

LDFLAGS ?= -nostdlib \
	-L$(IDF_PATH)/lib \
	-L$(IDF_PATH)/ld \
	$(addprefix -L$(BUILD_DIR_BASE)/,$(COMPONENTS) $(SRCDIRS)) \
	-u call_user_start_cpu0	\
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
CPPFLAGS := -DESP_PLATFORM $(CPPFLAGS)

# Warnings-related flags relevant both for C and C++
COMMON_WARNING_FLAGS = -Wall -Werror \
	-Wno-error=unused-function \
	-Wno-error=unused-but-set-variable \
	-Wno-error=unused-variable \
	-Wno-error=deprecated-declarations

# Flags which control code generation and dependency generation, both for C and C++
COMMON_FLAGS = \
	-ffunction-sections -fdata-sections \
	-fstrict-volatile-bitfields \
	-mlongcalls \
	-nostdlib \
	-MMD -MP

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
	$(COMMON_WARNING_FLAGS) \
	$(CFLAGS))

# List of flags to pass to C++ compiler
# If any flags are defined in application Makefile, add them at the end.
CXXFLAGS := $(strip \
	-std=gnu++11 \
	-fno-exceptions \
	-fno-rtti \
	$(OPTIMIZATION_FLAGS) \
	$(COMMON_FLAGS) \
	$(COMMON_WARNING_FLAGS) \
	$(CXXFLAGS))

export CFLAGS CPPFLAGS CXXFLAGS

#Set host compiler and binutils
HOSTCC := $(CC)
HOSTLD := $(LD)
HOSTAR := $(AR)
HOSTOBJCOPY := $(OBJCOPY)
export HOSTCC HOSTLD HOSTAR HOSTOBJCOPY

#Set target compiler. Defaults to whatever the user has
#configured as prefix + yer olde gcc commands
CC := $(call dequote,$(CONFIG_TOOLPREFIX))gcc
CXX := $(call dequote,$(CONFIG_TOOLPREFIX))c++
LD := $(call dequote,$(CONFIG_TOOLPREFIX))ld
AR := $(call dequote,$(CONFIG_TOOLPREFIX))ar
OBJCOPY := $(call dequote,$(CONFIG_TOOLPREFIX))objcopy
export CC CXX LD AR OBJCOPY

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

ifndef IS_BOOTLOADER_BUILD
# once we know component paths, we can include the config generation targets
#
# (bootloader build doesn't need this, config is exported from top-level)
include $(IDF_PATH)/make/project_config.mk
endif

# A "component" library is any library in the LDFLAGS where
# the name of the library is also a name of the component
APP_LIBRARIES = $(patsubst -l%,%,$(filter -l%,$(LDFLAGS)))
COMPONENT_LIBRARIES = $(filter $(notdir $(COMPONENT_PATHS_BUILDABLE)),$(APP_LIBRARIES))

# ELF depends on the library archive files for COMPONENT_LIBRARIES
# the rules to build these are emitted as part of GenerateComponentTarget below
$(APP_ELF): $(foreach libcomp,$(COMPONENT_LIBRARIES),$(BUILD_DIR_BASE)/$(libcomp)/lib$(libcomp).a)
	$(summary) LD $(notdir $@)
	$(Q) $(CC) $(LDFLAGS) -o $@ -Wl,-Map=$(APP_MAP)

# Generation of $(APP_BIN) from $(APP_ELF) is added by the esptool
# component's Makefile.projbuild
app: $(APP_BIN)
	@echo "App built. Default flash app command is:"
	@echo $(ESPTOOLPY_WRITE_FLASH) $(CONFIG_APP_OFFSET) $(APP_BIN)

all_binaries: $(APP_BIN)

$(BUILD_DIR_BASE):
	mkdir -p $(BUILD_DIR_BASE)

# Macro for the recursive sub-make for each component
# $(1) - component directory
# $(2) - component name only
#
# Is recursively expanded by the GenerateComponentTargets macro
define ComponentMake
$(Q) +$(MAKE) -C $(BUILD_DIR_BASE)/$(2) -f $(1)/component.mk COMPONENT_PATH=$(1) COMPONENT_BUILD_DIR=$(BUILD_DIR_BASE)/$(2)
endef

define GenerateComponentTargets
# $(1) - path to component dir
# $(2) - name of component
.PHONY: $(2)-build $(2)-clean

$(2)-build:
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

# add a target to generate the component_project_vars.mk files
# that are used to inject variables into project make pass (see
# component_project_vars.mk target in component_common.mk).
$(BUILD_DIR_BASE)/$(2)/component_project_vars.mk: $(1)/component.mk $(COMMON_MAKEFILES) $(SDKCONFIG) | $(BUILD_DIR_BASE)/$(2)
	$(call ComponentMake,$(1),$(2)) component_project_vars.mk
endef

$(foreach component,$(COMPONENT_PATHS_BUILDABLE),$(eval $(call GenerateComponentTargets,$(component),$(notdir $(component)))))

app-clean: $(addsuffix -clean,$(notdir $(COMPONENT_PATHS_BUILDABLE)))
	$(summary) RM $(APP_ELF)
	$(Q) rm -f $(APP_ELF) $(APP_BIN) $(APP_MAP)

clean: app-clean


