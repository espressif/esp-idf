COMPONENT_ADD_LDFLAGS := $(COMPONENT_PATH)/lib/libc.a $(COMPONENT_PATH)/lib/libm.a -lnewlib

COMPONENT_ADD_INCLUDEDIRS := include platform_include

include $(IDF_PATH)/make/component_common.mk
