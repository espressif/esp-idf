COMPONENT_ADD_INCLUDEDIRS := include esp32 include/esp32
COMPONENT_SRCDIRS := patches .

ifdef IS_BOOTLOADER_BUILD
COMPONENT_OBJEXCLUDE := patches/esp_rom_longjmp.o
endif

#Linker scripts used to link the final application.
#Warning: These linker scripts are only used when the normal app is compiled; the bootloader
#specifies its own scripts.
LINKER_SCRIPTS += esp32.rom.ld \
                  esp32.rom.libgcc.ld \
                  esp32.rom.syscalls.ld \
                  esp32.rom.newlib-data.ld \
                  esp32.rom.api.ld

#SPI-RAM incompatible functions can be used in when the SPI RAM
#workaround is not enabled.
ifndef CONFIG_SPIRAM_CACHE_WORKAROUND
LINKER_SCRIPTS += esp32.rom.newlib-funcs.ld

ifdef CONFIG_ESP32_REV_MIN_3
LINKER_SCRIPTS += esp32.rom.eco3.ld
endif

# Include in newlib nano from ROM only if SPIRAM cache workaround is disabled
ifdef CONFIG_NEWLIB_NANO_FORMAT
LINKER_SCRIPTS += esp32.rom.newlib-nano.ld
endif

endif #CONFIG_SPIRAM_CACHE_WORKAROUND

ifndef CONFIG_SPI_FLASH_ROM_DRIVER_PATCH
LINKER_SCRIPTS += esp32.rom.spiflash.ld
endif

ifndef CONFIG_SDK_TOOLCHAIN_SUPPORTS_TIME_WIDE_64_BITS
# If SDK_TOOLCHAIN_SUPPORTS_TIME_WIDE_64_BITS option is defined
# then all time functions from the ROM memory will not be linked.
# Instead, those functions can be used from the toolchain by ESP-IDF.
LINKER_SCRIPTS += esp32.rom.newlib-time.ld
endif

COMPONENT_ADD_LDFLAGS += -L $(COMPONENT_PATH)/esp32/ld \
                         $(addprefix -T ,$(LINKER_SCRIPTS))
ifndef IS_BOOTLOADER_BUILD
COMPONENT_ADD_LDFLAGS += -l$(COMPONENT_NAME) -Wl,--wrap=longjmp
endif

COMPONENT_ADD_LINKER_DEPS += $(addprefix esp32/ld/, $(LINKER_SCRIPTS))
