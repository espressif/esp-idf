#
# Main bootloader Makefile.
#
# This is basically the same as a component makefile, but in the case of the bootloader
# we pull in bootloader-specific linker arguments.
#

LINKER_SCRIPTS := esp32.bootloader.ld $(IDF_PATH)/components/esp32/ld/esp32.rom.ld

COMPONENT_ADD_LDFLAGS := -L $(COMPONENT_PATH) -lmain $(addprefix -T ,$(LINKER_SCRIPTS))

COMPONENT_ADD_LINKER_DEPS := $(LINKER_SCRIPTS)
