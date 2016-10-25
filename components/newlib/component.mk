COMPONENT_ADD_LDFLAGS := $(abspath lib/libc.a) $(abspath lib/libm.a) -lnewlib

COMPONENT_ADD_INCLUDEDIRS := include platform_include

include $(IDF_PATH)/make/component_common.mk
