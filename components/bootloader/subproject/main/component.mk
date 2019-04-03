#
# Main bootloader Makefile.
#
# This is basically the same as a component makefile, but in the case of the bootloader
# we pull in bootloader-specific linker arguments.
#

LINKER_SCRIPTS := \
    $(COMPONENT_PATH)/ld/$(IDF_TARGET)/bootloader.ld \
    $(COMPONENT_PATH)/ld/$(IDF_TARGET)/bootloader.rom.ld \
    $(IDF_PATH)/components/esp_rom/$(IDF_TARGET)/ld/$(IDF_TARGET).rom.ld \
    $(IDF_PATH)/components/esp_rom/$(IDF_TARGET)/ld/$(IDF_TARGET).rom.newlib-funcs.ld \
    $(IDF_PATH)/components/$(IDF_TARGET)/ld/$(IDF_TARGET).peripherals.ld

# SPI driver patch for ROM is only needed in ESP32
ifdef CONFIG_IDF_TARGET_ESP32
    ifndef CONFIG_SPI_FLASH_ROM_DRIVER_PATCH
        LINKER_SCRIPTS += $(IDF_PATH)/components/esp_rom/$(IDF_TARGET)/ld/$(IDF_TARGET).rom.spiflash.ld
    endif
endif

COMPONENT_ADD_LDFLAGS += -L $(COMPONENT_PATH) $(addprefix -T ,$(LINKER_SCRIPTS))

COMPONENT_ADD_LINKER_DEPS := $(LINKER_SCRIPTS)
