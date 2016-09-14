#
# Component Makefile
#
# This Makefile should, at the very least, just include $(IDF_PATH)/make/component_common.mk. By default, 
# this will take the sources in this directory, compile them and link them into 
# lib(subdirectory_name).a in the build directory. This behaviour is entirely configurable,
# please read the esp-idf build system document if you need to do this.
#
-include $(PROJECT_PATH)/build/include/config/auto.conf

COMPONENT_SRCDIRS := . hwcrypto

LIBS := crypto core net80211 phy rtc pp wpa wps

ifeq ($(CONFIG_MEMMAP_BT),y)
    ifeq ($(CONFIG_MEMMAP_TRACEMEM),y)
        LINKER_SCRIPTS = -T esp32.bt.trace.ld
    else
        LINKER_SCRIPTS = -T esp32.bt.ld
    endif
else
    ifeq ($(CONFIG_MEMMAP_TRACEMEM),y)
        LINKER_SCRIPTS = -T esp32.trace.ld
    else
        LINKER_SCRIPTS = -T esp32.ld
    endif
endif

LINKER_SCRIPTS += -T esp32.common.ld -T esp32.rom.ld

COMPONENT_ADD_LDFLAGS := -lesp32 \
                           $(abspath libhal.a) \
                           -L$(abspath lib) \
                           $(addprefix -l,$(LIBS)) \
                          -L $(abspath ld) \
                          $(LINKER_SCRIPTS)

include $(IDF_PATH)/make/component_common.mk

ALL_LIB_FILES := $(patsubst %,$(COMPONENT_PATH)/lib/lib%.a,$(LIBS))

# automatically trigger a git submodule update
# if any libraries are missing
$(eval $(call SubmoduleRequiredForFiles,$(ALL_LIB_FILES)))

# this is a hack to make sure the app is re-linked if the binary
# libraries change or are updated. If they change, the main esp32
# library will be rebuild by AR andthis will trigger a re-linking of
# the entire app.
#
# It would be better for components to be able to expose any of these
# non-standard dependencies via get_variable, but this will do for now.
$(COMPONENT_LIBRARY): $(ALL_LIB_FILES)
