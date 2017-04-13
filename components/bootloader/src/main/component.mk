#
# Main bootloader Makefile.
#
# This is basically the same as a component makefile, but in the case of the bootloader
# we pull in bootloader-specific linker arguments.
#

LINKER_SCRIPTS := \
	esp32.bootloader.ld \
	$(IDF_PATH)/components/esp32/ld/esp32.rom.ld \
	$(IDF_PATH)/components/esp32/ld/esp32.peripherals.ld \
	esp32.bootloader.rom.ld

ifndef CONFIG_SPI_FLASH_ROM_DRIVER_PATCH
LINKER_SCRIPTS += $(IDF_PATH)/components/esp32/ld/esp32.rom.spiflash.ld
endif

COMPONENT_ADD_LDFLAGS := -L $(COMPONENT_PATH) -lmain $(addprefix -T ,$(LINKER_SCRIPTS))

COMPONENT_ADD_LINKER_DEPS := $(LINKER_SCRIPTS)
