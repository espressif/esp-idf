#
# Main bootloader Makefile.
#
# This is basically the same as a component makefile, but in the case of the bootloader
# we pull in bootloader-specific linker arguments.
#

COMPONENT_ADD_LDFLAGS := -L $(COMPONENT_PATH) -lmain -T esp32.bootloader.ld -T $(IDF_PATH)/components/esp32/ld/esp32.rom.ld

