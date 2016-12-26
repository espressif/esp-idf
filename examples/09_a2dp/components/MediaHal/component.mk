#
# Component Makefile
#
# This Makefile should, at the very least, just include $(SDK_PATH)/Makefile. By default, 
# this will take the sources in the src/ directory, compile them and link them into 
# lib(subdirectory_name).a in the build directory. This behaviour is entirely configurable,
# please read the SDK documents if you need to do this.
#

COMPONENT_ADD_INCLUDEDIRS := include include/AP80 include/ES8388 include/driver \
                             ../misc/include

COMPONENT_SRCDIRS :=  AP80 ES8388 Driver test .

include $(IDF_PATH)/make/component_common.mk
