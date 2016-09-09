#
# Main Makefile. This is basically the same as a component makefile.
#
# This Makefile should, at the very least, just include $(IDF_PATH)/make/component_common.mk. By default, 
# this will take the sources in the src/ directory, compile them and link them into 
# lib(subdirectory_name).a in the build directory. This behaviour is entirely configurable,
# please read the esp-idf build system document if you need to do this.
#

COMPONENT_ADD_LDFLAGS := -L $(abspath .) -lmain -T esp32.bootloader.ld -T $(IDF_PATH)/components/esp32/ld/esp32.rom.ld 
COMPONENT_EXTRA_INCLUDES := $(IDF_PATH)/components/esp32/include 

include $(IDF_PATH)/make/component_common.mk
