#
# Component Makefile
#
# This Makefile should, at the very least, just include $(SDK_PATH)/Makefile. By default, 
# this will take the sources in this directory, compile them and link them into 
# lib(subdirectory_name).a in the build directory. This behaviour is entirely configurable,
# please read the SDK documents if you need to do this.
#
COMPONENT_ADD_INCLUDEDIRS := port/include include/expat

COMPONENT_SRCDIRS := library port

CFLAGS += -Wno-unused-function -DHAVE_EXPAT_CONFIG_H

include $(IDF_PATH)/make/component_common.mk
