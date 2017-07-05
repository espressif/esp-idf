#
# Component Makefile
#

#ifdef IS_BOOTLOADER_BUILD
CFLAGS += -DBOOTLOADER_BUILD
#endif

COMPONENT_SRCDIRS := . hwcrypto
LIBS := core rtc
ifdef CONFIG_PHY_ENABLED # BT || WIFI
LIBS += phy coexist
endif
ifdef CONFIG_WIFI_ENABLED
LIBS += net80211 pp wpa smartconfig coexist wps wpa2
endif

LINKER_SCRIPTS += esp32.common.ld esp32.rom.ld esp32.peripherals.ld

ifeq ("$(CONFIG_NEWLIB_NANO_FORMAT)","y")
LINKER_SCRIPTS += esp32.rom.nanofmt.ld
endif

ifndef CONFIG_SPI_FLASH_ROM_DRIVER_PATCH
LINKER_SCRIPTS += esp32.rom.spiflash.ld
endif

COMPONENT_ADD_LDFLAGS := -lesp32 \
                         $(COMPONENT_PATH)/libhal.a \
                         -L$(COMPONENT_PATH)/lib \
                         $(addprefix -l,$(LIBS)) \
                         -L $(COMPONENT_PATH)/ld \
                         -T esp32_out.ld \
                         $(addprefix -T ,$(LINKER_SCRIPTS))

ALL_LIB_FILES := $(patsubst %,$(COMPONENT_PATH)/lib/lib%.a,$(LIBS))

COMPONENT_SUBMODULES += lib

# final linking of project ELF depends on all binary libraries, and
# all linker scripts (except esp32_out.ld, as this is code generated here.)
COMPONENT_ADD_LINKER_DEPS := $(ALL_LIB_FILES) $(addprefix ld/,$(LINKER_SCRIPTS))

# Preprocess esp32.ld linker script into esp32_out.ld
#
# The library doesn't really depend on esp32_out.ld, but it
# saves us from having to add the target to a Makefile.projbuild
$(COMPONENT_LIBRARY): esp32_out.ld

esp32_out.ld: $(COMPONENT_PATH)/ld/esp32.ld ../include/sdkconfig.h
	$(CC) -I ../include -C -P -x c -E $< -o $@

COMPONENT_EXTRA_CLEAN := esp32_out.ld
