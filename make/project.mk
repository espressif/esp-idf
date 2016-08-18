#
# Main Project Makefile
# This Makefile is included directly from the user project Makefile in order to call the Makefiles of all the
#   components (in a separate make process) to build all the libraries, then links them together
#   into the final file. If so, PWD is the project dir (we assume).
#

#
# This Makefile requires the environment variable SDK_PATH to be set to the directory where this
# Makefile is located.
#

.PHONY: build-components menuconfig all build clean
all: project

# disable built-in make rules, makes debugging saner
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

#Component directories. These directories are searched for components.
#The project Makefile can override these component dirs, or define extra component directories.
COMPONENT_DIRS ?= $(PROJECT_PATH)/components $(EXTRA_COMPONENT_DIRS) $(SDK_PATH)/components
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

#A component is buildable if it has a Makefile; we assume that a 'make -C $(component dir) build' results in a
#lib$(componentname).a. 
COMPONENT_PATHS_BUILDABLE := $(foreach cp,$(COMPONENT_PATHS),$(if $(wildcard $(cp)/Makefile),$(cp)))

# Assemble global list of include dirs (COMPONENT_INCLUDES), and
# LDFLAGS args (COMPONENT_LDFLAGS) supplied by each component.
COMPONENT_INCLUDES :=
COMPONENT_LDFLAGS :=
#
# Also add any inter-component dependencies for each component.

# Extract a variable from a child make process
#
# $(1) - path to directory to invoke make in
# $(2) - name of variable to print via the getvariable target (passed in GET_VARIABLE)
#
# needs 'sed' processing of stdout because make sometimes echoes other stuff on stdout,
# even if asked not to.
#
# Debugging this? Replace $(shell with $(error and you'll see the full command as-run.
define GetVariable
$(shell "$(MAKE)" -s --no-print-directory -C $(1) get_variable PROJECT_PATH=$(PROJECT_PATH) GET_VARIABLE=$(2) | sed -En "s/^$(2)=(.+)/\1/p" )
endef

ifeq ("$(COMPONENT_INCLUDES)","")
COMPONENT_INCLUDES := $(abspath $(foreach comp,$(COMPONENT_PATHS_BUILDABLE),$(addprefix $(comp)/, \
	$(call GetVariable,$(comp),COMPONENT_ADD_INCLUDEDIRS))))
endif

#Also add project include path, for sdk includes
COMPONENT_INCLUDES += $(PROJECT_PATH)/build/include/
export COMPONENT_INCLUDES

#COMPONENT_LDFLAGS has a list of all flags that are needed to link the components together. It's collected
#in the same way as COMPONENT_INCLUDES is.
ifeq ("$(COMPONENT_LDFLAGS)","")
COMPONENT_LDFLAGS := $(foreach comp,$(COMPONENT_PATHS_BUILDABLE), \
	$(call GetVariable,$(comp),COMPONENT_ADD_LDFLAGS))
export COMPONENT_LDFLAGS
endif

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
-include $(SDK_PATH)/make/common.mk

#Set host compiler and binutils
HOSTCC := $(CC)
HOSTLD := $(LD)
HOSTAR := $(AR)
HOSTOBJCOPY := $(OBJCOPY)

#Set target compiler. Defaults to whatever the user has
#configured as prefix + yer olde gcc commands
CC := $(call dequote,$(CONFIG_TOOLPREFIX))gcc
CXX := $(call dequote,$(CONFIG_TOOLPREFIX))c++
LD := $(call dequote,$(CONFIG_TOOLPREFIX))ld
AR := $(call dequote,$(CONFIG_TOOLPREFIX))ar
OBJCOPY := $(call dequote,$(CONFIG_TOOLPREFIX))objcopy
export CC CXX LD AR OBJCOPY

PYTHON=$(call dequote,$(CONFIG_PYTHON))

PROJECT_ELF:=$(BUILD_DIR_BASE)/$(PROJECT_NAME).elf
PROJECT_MAP:=$(PROJECT_ELF:.elf=.map)
PROJECT_BIN:=$(PROJECT_ELF:.elf=.bin)

# Include any Makefile.projbuild file letting components add
# configuration at the project level
define includeProjBuildMakefile
COMPONENT_PATH := $(1)
-include $(1)/Makefile.projbuild
endef
$(foreach componentpath,$(COMPONENT_PATHS),$(eval $(call includeProjBuildMakefile,$(componentpath))))

# once we know component paths, we can include the config
include $(SDK_PATH)/make/project_config.mk

# ELF depends on the -build target of every component
$(PROJECT_ELF): $(addsuffix -build,$(notdir $(COMPONENT_PATHS_BUILDABLE)))
	$(vecho) LD $(notdir $@)
	$(Q) $(CC) $(LDFLAGS) -o $@ -Wl,-Map=$(PROJECT_MAP)

# Generation of $(PROJECT_BIN) from $(PROJECT_ELF) is added by the esptool
# component's Makefile.projbuild

project: $(PROJECT_BIN)
	@echo "App built. Default flash app command is:"
	@echo $(PROJECT_FLASH_COMMAND) # PROJECT_FLASH_COMMAND is set in esptool_py's Makefile.projbuild

$(BUILD_DIR_BASE):
	mkdir -p $(BUILD_DIR_BASE)

define GenerateComponentTarget
# $(1) - path to component dir
# $(2) - target to generate (build, clean)
# $(3) - optional dependencies to add
.PHONY: $(notdir $(1))-$(2)
$(notdir $(1))-$(2): $(3) | $(BUILD_DIR_BASE)/$(notdir $(1))
	@+$(MAKE) -C $(BUILD_DIR_BASE)/$(notdir $(1)) -f $(1)/Makefile COMPONENT_BUILD_DIR=$(BUILD_DIR_BASE)/$(notdir $(1)) $(2)
endef

define GenerateComponentBuildDirTarget
# $(1) - path to component dir
$(BUILD_DIR_BASE)/$(notdir $(1)):
	@mkdir -p $(BUILD_DIR_BASE)/$(notdir $(1))
endef

$(foreach component,$(COMPONENT_PATHS_BUILDABLE),$(eval $(call GenerateComponentBuildDirTarget,$(component))))

$(foreach component,$(COMPONENT_PATHS_BUILDABLE),$(eval $(call GenerateComponentTarget,$(component),build,$(PROJECT_PATH)/build/include/sdkconfig.h)))
$(foreach component,$(COMPONENT_PATHS_BUILDABLE),$(eval $(call GenerateComponentTarget,$(component),clean)))

clean: $(addsuffix -clean,$(notdir $(COMPONENT_PATHS_BUILDABLE))) $(EXTRA_CLEAN_TARGETS)
	$(vecho) RM $(PROJECT_ELF)
	$(Q) rm -f $(PROJECT_ELF) $(PROJECT_BIN) $(PROJECT_MAP)
	$(Q) rm -rf $(PROJECT_PATH)/build/include/config $(PROJECT_PATH)/build/include/sdkconfig.h


