#
# Component Makefile
#

#COMPONENT_ADD_INCLUDEDIRS := 

COMPONENT_ADD_INCLUDEDIRS := include

CFLAGS += -Wno-error=unused-label -Wno-error=return-type -Wno-error=missing-braces -Wno-error=pointer-sign -Wno-error=parentheses

LIBS := btdm_app

COMPONENT_ADD_LDFLAGS := -lbt -L$(abspath lib) \
                           $(addprefix -l,$(LIBS)) \
                          $(LINKER_SCRIPTS)

include $(IDF_PATH)/make/component_common.mk

ALL_LIB_FILES := $(patsubst %,$(COMPONENT_PATH)/lib/lib%.a,$(LIBS))
$(COMPONENT_LIBRARY): $(ALL_LIB_FILES)

# automatically trigger a git submodule update if BT library is missing
$(eval $(call SubmoduleCheck,$(ALL_LIB_FILES),$(COMPONENT_PATH)/lib))
