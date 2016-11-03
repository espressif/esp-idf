# Component common makefile
#
# This Makefile gets included in the Makefile of all the components to set the correct include paths etc.
# PWD is the build directory of the component and the top Makefile is the one in the
# component source dir.
#
# The way the Makefile differentiates between those two is by looking at the environment
# variable PROJECT_PATH. If this is set (to the basepath of the project), we're building a
# component and its Makefile has included this makefile. If not, we're building the entire project.
#

#
# This Makefile requires the environment variable IDF_PATH to be set
# to the top-level directory where ESP-IDF is located (the directory
# containing this 'make' directory).
#

ifeq ("$(PROJECT_PATH)","")
$(error Make was invoked from $(CURDIR). However please do not run make from the sdk or a component directory; invoke make from the project directory. See the ESP-IDF README for details.)
endif

# Find the path to the component
COMPONENT_PATH := $(abspath $(dir $(firstword $(MAKEFILE_LIST))))
export COMPONENT_PATH

include $(IDF_PATH)/make/common.mk

#Some of these options are overridable by the component's component.mk Makefile

#Name of the component
COMPONENT_NAME ?= $(lastword $(subst /, ,$(realpath $(COMPONENT_PATH))))

#Absolute path of the .a file
COMPONENT_LIBRARY := lib$(COMPONENT_NAME).a

#Source dirs a component has. Default to root directory of component.
COMPONENT_SRCDIRS ?= .

#Names of binary files to embed as symbols in the component library
COMPONENT_EMBED_FILES ?= 

#Object files which need to be added to the library
#By default we take all .c/.S files in the component directory.
ifeq ("$(COMPONENT_OBJS)", "")
#Find all source files in all COMPONENT_SRCDIRS
COMPONENT_OBJS := $(foreach compsrcdir,$(COMPONENT_SRCDIRS),$(patsubst %.c,%.o,$(wildcard $(COMPONENT_PATH)/$(compsrcdir)/*.c)))
COMPONENT_OBJS += $(foreach compsrcdir,$(COMPONENT_SRCDIRS),$(patsubst %.cpp,%.o,$(wildcard $(COMPONENT_PATH)/$(compsrcdir)/*.cpp)))
COMPONENT_OBJS += $(foreach compsrcdir,$(COMPONENT_SRCDIRS),$(patsubst %.S,%.o,$(wildcard $(COMPONENT_PATH)/$(compsrcdir)/*.S)))
#Make relative by removing COMPONENT_PATH from all found object paths
COMPONENT_OBJS := $(patsubst $(COMPONENT_PATH)/%,%,$(COMPONENT_OBJS))
endif

#Object files with embedded binaries to add to the component library
#Correspond to the files named in COMPONENT_EMBED_FILES & COMPONENT_EMBED_TXTFILES
COMPONENT_EMBED_OBJS ?= $(addsuffix .bin.o,$(COMPONENT_EMBED_FILES)) $(addsuffix .txt.o,$(COMPONENT_EMBED_TXTFILES))

#By default, include only the include/ dir.
COMPONENT_ADD_INCLUDEDIRS ?= include
COMPONENT_ADD_LDFLAGS ?= -l$(COMPONENT_NAME)

#If we're called to compile something, we'll get passed the COMPONENT_INCLUDES
#variable with all the include dirs from all the components in random order. This
#means we can accidentally grab a header from another component before grabbing our own.
#To make sure that does not happen, re-order the includes so ours come first.
OWN_INCLUDES:=$(abspath $(addprefix $(COMPONENT_PATH)/,$(COMPONENT_ADD_INCLUDEDIRS) $(COMPONENT_PRIV_INCLUDEDIRS)))
COMPONENT_INCLUDES := $(OWN_INCLUDES) $(filter-out $(OWN_INCLUDES),$(COMPONENT_INCLUDES))

#This target is used to collect variable values from inside project.mk
# see project.mk GetVariable macro for details.
get_variable:
	@echo "$(GET_VARIABLE)=$(call $(GET_VARIABLE)) "

#Targets for build/clean. Use builtin recipe if component Makefile
#hasn't defined its own.
ifeq ("$(COMPONENT_OWNBUILDTARGET)", "")
build: $(COMPONENT_LIBRARY)
	@mkdir -p $(COMPONENT_SRCDIRS)

#Build the archive. We remove the archive first, otherwise ar will get confused if we update
#an archive when multiple filenames have the same name (src1/test.o and src2/test.o)
$(COMPONENT_LIBRARY): $(COMPONENT_OBJS) $(COMPONENT_EMBED_OBJS)
	$(summary) AR $@
	$(Q) rm -f $@
	$(Q) $(AR) cru $@ $^
endif

ifeq ("$(COMPONENT_OWNCLEANTARGET)", "")
clean:
	$(summary) RM $(COMPONENT_LIBRARY) $(COMPONENT_OBJS) $(COMPONENT_OBJS:.o=.d) $(COMPONENT_EMBED_OBJS) $(COMPONENT_EXTRA_CLEAN)
	$(Q) rm -f $(COMPONENT_LIBRARY) $(COMPONENT_OBJS) $(COMPONENT_OBJS:.o=.d) $(COMPONENT_EMBED_OBJS) $(COMPONENT_EXTRA_CLEAN)
endif

#Include all dependency files already generated
-include $(COMPONENT_OBJS:.o=.d)

#This pattern is generated for each COMPONENT_SRCDIR to compile the files in it.
define GenerateCompileTargets
# $(1) - directory containing source files, relative to $(COMPONENT_PATH)
$(1)/%.o: $$(COMPONENT_PATH)/$(1)/%.c | $(1)
	$$(summary) CC $$@
	$$(Q) $$(CC) $$(CFLAGS) $(CPPFLAGS) $$(addprefix -I ,$$(COMPONENT_INCLUDES)) $$(addprefix -I ,$$(COMPONENT_EXTRA_INCLUDES)) -I$(1) -c $$< -o $$@

$(1)/%.o: $$(COMPONENT_PATH)/$(1)/%.cpp | $(1)
	$$(summary) CXX $$@
	$$(Q) $$(CXX) $$(CXXFLAGS) $(CPPFLAGS) $$(addprefix -I,$$(COMPONENT_INCLUDES)) $$(addprefix -I,$$(COMPONENT_EXTRA_INCLUDES)) -I$(1) -c $$< -o $$@

$(1)/%.o: $$(COMPONENT_PATH)/$(1)/%.S | $(1)
	$$(summary) AS $$@
	$$(Q) $$(CC) $$(CFLAGS) $(CPPFLAGS) $$(addprefix -I ,$$(COMPONENT_INCLUDES)) $$(addprefix -I ,$$(COMPONENT_EXTRA_INCLUDES)) -I$(1) -c $$< -o $$@

endef

define GenerateBuildDirTarget
# CWD is build dir, create the build subdirectory $(1) if it doesn't exist
$(1):
	@mkdir -p $(1)
endef

#Generate all the compile target recipes & the build directory recipes
$(foreach srcdir,$(COMPONENT_SRCDIRS), $(eval $(call GenerateCompileTargets,$(srcdir)))  $(eval $(call GenerateBuildDirTarget,$(srcdir))))

## Support for embedding binary files into the ELF as symbols

OBJCOPY_EMBED_ARGS := --input binary --output elf32-xtensa-le --binary-architecture xtensa --rename-section .data=.rodata.embedded

# Generate pattern for embedding text or binary files into the app
# $(1) is name of file (as relative path inside component)
# $(2) is txt or bin depending on file contents
#
# txt files are null-terminated before being embedded (otherwise
# identical behaviour.)
#
# Files are temporarily copied to the build directory before objcopy,
# because objcopy generates the symbol name from the full command line
# path to the input file.
define GenerateEmbedTarget
$(1).$(2).o: $(call resolvepath,$(1),$(COMPONENT_PATH)) | $$(dir $(1))
	$$(summary) EMBED $$@
	$$(Q) $(if $(filter-out $$(notdir $$(abspath $$<)),$$(abspath $$(notdir $$<))), cp $$< $$(notdir $$<) )  # copy input file to build dir, unless already in build dir
	$$(Q) $(if $(subst bin,,$(2)),echo -ne '\0' >> $$(notdir $$<) )  # trailing NUL byte on text output
	$$(Q) $$(OBJCOPY) $(OBJCOPY_EMBED_ARGS) $$(notdir $$<) $$@
	$$(Q) rm $$(notdir $$<)
endef

# generate targets to embed binary & text files
$(foreach binfile,$(COMPONENT_EMBED_FILES), $(eval $(call GenerateEmbedTarget,$(binfile),bin)))

$(foreach txtfile,$(COMPONENT_EMBED_TXTFILES), $(eval $(call GenerateEmbedTarget,$(txtfile),txt)))

# generate targets to create binary embed directories
$(foreach bindir,$(sort $(dir $(COMPONENT_EMBED_FILES))), $(eval $(call GenerateBuildDirTarget,$(bindir))))
