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

#The directory where we put all objects/libraries/binaries. The project Makefile can
#configure this if needed.
BUILD_DIR_BASE ?= $(PROJECT_PATH)/build
export BUILD_DIR_BASE

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
#
# Also add any inter-component dependencies for each component.

# Extract a variable from a child make process
#
# $(1) - path to directory to invoke make in
# $(2) - name of variable to print via the get_variable target (passed in GET_VARIABLE)
#
# needs 'sed' processing of stdout because make sometimes echoes other stuff on stdout,
# even if asked not to.
#
# Debugging this? Replace $(shell with $(error and you'll see the full command as-run.
define GetVariable
$(shell "$(MAKE)" -s --no-print-directory -C $(1) -f component.mk get_variable PROJECT_PATH=$(PROJECT_PATH) GET_VARIABLE=$(2) | sed -En "s/^$(2)=(.+)/\1/p" )
endef

COMPONENT_INCLUDES := $(abspath $(foreach comp,$(COMPONENT_PATHS_BUILDABLE),$(addprefix $(comp)/, \
	$(call GetVariable,$(comp),COMPONENT_ADD_INCLUDEDIRS))))

#Also add project include path, for sdk includes
COMPONENT_INCLUDES += $(abspath $(BUILD_DIR_BASE)/include/)
export COMPONENT_INCLUDES

#COMPONENT_LDFLAGS has a list of all flags that are needed to link the components together. It's collected
#in the same way as COMPONENT_INCLUDES is.
COMPONENT_LDFLAGS := $(foreach comp,$(COMPONENT_PATHS_BUILDABLE), \
	$(call GetVariable,$(comp),COMPONENT_ADD_LDFLAGS))
export COMPONENT_LDFLAGS

# Generate component dependency targets from dependencies lists
# each component gains a target of its own <name>-build with dependencies
# of the names of any other components (-build) that need building first
#
# the actual targets (that invoke submakes) are generated below by
# GenerateComponentTarget macro.
define GenerateComponentDependencies
# $(1) = component path
.PHONY: $$(notdir $(1))
$$(notdir $(1))-build: $(addsuffix -build,$(call GetVariable,$(1),COMPONENT_DEPENDS))
endef
$(foreach comp,$(COMPONENT_PATHS_BUILDABLE), $(eval $(call GenerateComponentDependencies,$(comp))))

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

# once we know component paths, we can include the config
include $(IDF_PATH)/make/project_config.mk

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

define GenerateComponentPhonyTarget
# $(1) - path to component dir
# $(2) - target to generate (build, clean)
.PHONY: $(notdir $(1))-$(2)
$(notdir $(1))-$(2): | $(BUILD_DIR_BASE)/$(notdir $(1))
	$(Q) +$(MAKE) -C $(BUILD_DIR_BASE)/$(notdir $(1)) -f $(1)/component.mk COMPONENT_BUILD_DIR=$(BUILD_DIR_BASE)/$(notdir $(1)) $(2)
endef

define GenerateComponentTargets
# $(1) - path to component dir
$(BUILD_DIR_BASE)/$(notdir $(1)):
	@mkdir -p $(BUILD_DIR_BASE)/$(notdir $(1))

# tell make it can build any component's library by invoking the recursive -build target
# (this target exists for all components even ones which don't build libraries, but it's
# only invoked for the targets whose libraries appear in COMPONENT_LIBRARIES and hence the
# APP_ELF dependencies.)
$(BUILD_DIR_BASE)/$(notdir $(1))/lib$(notdir $(1)).a: $(notdir $(1))-build
	$(details) "Target '$$^' responsible for '$$@'" # echo which build target built this file
endef

$(foreach component,$(COMPONENT_PATHS_BUILDABLE),$(eval $(call GenerateComponentTargets,$(component))))

$(foreach component,$(COMPONENT_PATHS_BUILDABLE),$(eval $(call GenerateComponentPhonyTarget,$(component),build)))
$(foreach component,$(COMPONENT_PATHS_BUILDABLE),$(eval $(call GenerateComponentPhonyTarget,$(component),clean)))

app-clean: $(addsuffix -clean,$(notdir $(COMPONENT_PATHS_BUILDABLE)))
	$(summary) RM $(APP_ELF)
	$(Q) rm -f $(APP_ELF) $(APP_BIN) $(APP_MAP)

clean: app-clean


