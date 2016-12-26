#
# Component Makefile
#
# This Makefile should, at the very least, just include $(SDK_PATH)/Makefile. By default, 
# this will take the sources in the src/ directory, compile them and link them into 
# lib(subdirectory_name).a in the build directory. This behaviour is entirely configurable,
# please read the SDK documents if you need to do this.
#
COMPONENT_ADD_INCLUDEDIRS := include player/inc recorder/inc \
                             ../media_hal/include \
                             ../misc/include

COMPONENT_SRCDIRS := player recorder

include $(IDF_PATH)/make/component_common.mk


