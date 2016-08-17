# Main SDK Makefile
# This Makefile gets included in the Makefile of all the components to set the correct include paths etc.
# PWD is the build directory of the component and the top Makefile is the one in the
# component source dir.
#
# The way the Makefile differentiates between those two is by looking at the environment
# variable PROJECT_PATH. If this is set (to the basepath of the project), we're building a
# component and its Makefile has included this makefile. If not, we're building the entire project.
#

#
# This Makefile requires the environment variable SDK_PATH to be set
# to the directory where the SDK (containing the make directory with
# this file) is located
#

ifeq ("$(PROJECT_PATH)","")
$(error Make was invoked from $(CURDIR). However please do not run make from the sdk or a component directory; invoke make from the project directory. See SDK README for details.)
endif

# Find the path to the component
COMPONENT_PATH := $(abspath $(dir $(firstword $(MAKEFILE_LIST))))
export COMPONENT_PATH

include $(SDK_PATH)/make/common.mk

#Some of these options are overridable by the components Makefile.

#Name of the component
COMPONENT_NAME ?= $(lastword $(subst /, ,$(realpath $(COMPONENT_PATH))))

#Absolute path of the .a file
COMPONENT_LIBRARY := lib$(COMPONENT_NAME).a

#Source dirs a component has. Default to root directory of component.
COMPONENT_SRCDIRS ?= .

#Object files which need to be linked into the library
#By default we take all .c/.S files in the component directory.
ifeq ("$(COMPONENT_OBJS)", "")
#Find all source files in all COMPONENT_SRCDIRS
COMPONENT_OBJS := $(foreach compsrcdir,$(COMPONENT_SRCDIRS),$(patsubst %.c,%.o,$(wildcard $(COMPONENT_PATH)/$(compsrcdir)/*.c)))
COMPONENT_OBJS += $(foreach compsrcdir,$(COMPONENT_SRCDIRS),$(patsubst %.cpp,%.o,$(wildcard $(COMPONENT_PATH)/$(compsrcdir)/*.cpp)))
COMPONENT_OBJS += $(foreach compsrcdir,$(COMPONENT_SRCDIRS),$(patsubst %.S,%.o,$(wildcard $(COMPONENT_PATH)/$(compsrcdir)/*.S)))
#Make relative by removing COMPONENT_PATH from all found object paths
COMPONENT_OBJS := $(patsubst $(COMPONENT_PATH)/%,%,$(COMPONENT_OBJS))
endif

#By default, include only the include/ dir.
COMPONENT_ADD_INCLUDEDIRS ?= include
COMPONENT_ADD_LDFLAGS ?= -l$(COMPONENT_NAME)

#If we're called to compile something, we'll get passed the COMPONENT_INCLUDES
#variable with all the include dirs from all the components in random order. This
#means we can accidentally grab a header from another component before grabbing our own.
#To make sure that does not happen, re-order the includes so ours come first.
OWN_INCLUDES:=$(abspath $(addprefix $(COMPONENT_PATH)/,$(COMPONENT_ADD_INCLUDEDIRS) $(COMPONENT_PRIV_INCLUDEDIRS)))
COMPONENT_INCLUDES := $(OWN_INCLUDES) $(filter-out $(OWN_INCLUDES),$(COMPONENT_INCLUDES))

#This target is used to collect variable values from inside the main makefile
get_variable:
	@echo "$(GET_VARIABLE)=$(call $(GET_VARIABLE)) "

#Targets for build/clean. Use builtin recipe if component Makefile
#hasn't defined its own.
ifeq ("$(COMPONENT_OWNBUILDTARGET)", "")
build: $(COMPONENT_LIBRARY)
	@mkdir -p $(COMPONENT_SRCDIRS)

#Build the archive. We remove the archive first, otherwise ar will get confused if we update
#an archive when multiple filenames have the same name (src1/test.o and src2/test.o)
$(COMPONENT_LIBRARY): $(COMPONENT_OBJS)
	$(vecho) AR $@
	$(Q) rm -f $@
	$(Q) $(AR) cru $@ $(COMPONENT_OBJS)
endif

ifeq ("$(COMPONENT_OWNCLEANTARGET)", "")
clean:
	$(vecho) RM $(COMPONENT_LIBRARY) $(COMPONENT_OBJS) $(COMPONENT_OBJS:.o=.d) $(COMPONENT_EXTRA_CLEAN)
	$(Q) rm -f $(COMPONENT_LIBRARY) $(COMPONENT_OBJS) $(COMPONENT_OBJS:.o=.d) $(COMPONENT_EXTRA_CLEAN)
endif

#Also generate dependency files
CFLAGS+=-MMD
CXXFLAGS+=-MMD
#Include all dependency files already generated
-include $(COMPONENT_OBJS:.o=.d)

#This pattern is generated for each COMPONENT_SRCDIR to compile the files in it.
define GenerateCompileTargets
# $(1) - directory containing source files, relative to $(COMPONENT_PATH)
$(1)/%.o: $$(COMPONENT_PATH)/$(1)/%.c | $(1)
	$$(vecho) CC $$@
	$$(Q) $$(CC) $$(CFLAGS) $$(addprefix -I ,$$(COMPONENT_INCLUDES)) $$(addprefix -I ,$$(COMPONENT_EXTRA_INCLUDES)) -I$(1) -c $$< -o $$@

$(1)/%.o: $$(COMPONENT_PATH)/$(1)/%.cpp | $(1)
	$$(vecho) CC $$@
	$$(Q) $$(CXX) $$(CXXFLAGS) $$(addprefix -I,$$(COMPONENT_INCLUDES)) $$(addprefix -I,$$(COMPONENT_EXTRA_INCLUDES)) -I$(1) -c $$< -o $$@

$(1)/%.o: $$(COMPONENT_PATH)/$(1)/%.S | $(1)
	$$(vecho) CC $$@
	$$(Q) $$(CC) $$(CFLAGS) $$(addprefix -I ,$$(COMPONENT_INCLUDES)) $$(addprefix -I ,$$(COMPONENT_EXTRA_INCLUDES)) -I$(1) -c $$< -o $$@

# CWD is build dir, create the build subdirectory if it doesn't exist
$(1):
	@mkdir -p $(1)
endef

#Generate all the compile target recipes
$(foreach srcdir,$(COMPONENT_SRCDIRS), $(eval $(call GenerateCompileTargets,$(srcdir))))
