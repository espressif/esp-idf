#
# Component Makefile
#

COMPONENT_SRCDIRS := . hwcrypto
LIBS ?=
ifndef CONFIG_NO_BLOBS
LIBS += core rtc net80211 pp wpa smartconfig coexist wps wpa2 phy
endif

#Linker scripts used to link the final application.
#Warning: These linker scripts are only used when the normal app is compiled; the bootloader
#specifies its own scripts.
LINKER_SCRIPTS += esp32.common.ld esp32.rom.ld esp32.peripherals.ld

#SPI-RAM incompatible functions can be used in when the SPI RAM 
#workaround is not enabled.
ifndef CONFIG_SPIRAM_CACHE_WORKAROUND
LINKER_SCRIPTS += esp32.rom.spiram_incompatible_fns.ld
endif

ifdef CONFIG_NEWLIB_NANO_FORMAT
LINKER_SCRIPTS += esp32.rom.nanofmt.ld
endif

ifndef CONFIG_SPI_FLASH_ROM_DRIVER_PATCH
LINKER_SCRIPTS += esp32.rom.spiflash.ld
endif

#ld_include_panic_highint_hdl is added as an undefined symbol because otherwise the 
#linker will ignore panic_highint_hdl.S as it has no other files depending on any
#symbols in it.
COMPONENT_ADD_LDFLAGS += $(COMPONENT_PATH)/libhal.a \
                         -L$(COMPONENT_PATH)/lib \
                         $(addprefix -l,$(LIBS)) \
                         -L $(COMPONENT_PATH)/ld \
                         -T esp32_out.ld \
                         -u ld_include_panic_highint_hdl \
                         $(addprefix -T ,$(LINKER_SCRIPTS))

#The cache workaround also needs a c++ standard library recompiled with the workaround.
ifdef CONFIG_SPIRAM_CACHE_WORKAROUND
COMPONENT_ADD_LDFLAGS += $(COMPONENT_PATH)/libstdcc++-cache-workaround.a
endif

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
