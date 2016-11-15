#
# Component Makefile
#

COMPONENT_SRCDIRS := . hwcrypto

LIBS := core net80211 phy rtc pp wpa smartconfig coexist wps

LINKER_SCRIPTS += -T esp32_out.ld -T esp32.common.ld -T esp32.rom.ld -T esp32.peripherals.ld

COMPONENT_ADD_LDFLAGS := -lesp32 \
                           $(COMPONENT_PATH)/libhal.a \
                           -L$(COMPONENT_PATH)/lib \
                           $(addprefix -l,$(LIBS)) \
                          -L $(COMPONENT_PATH)/ld \
                          $(LINKER_SCRIPTS)

ALL_LIB_FILES := $(patsubst %,$(COMPONENT_PATH)/lib/lib%.a,$(LIBS))

# automatically trigger a git submodule update
# if any libraries are missing
$(eval $(call SubmoduleCheck,$(ALL_LIB_FILES),$(COMPONENT_PATH)/lib))

# this is a hack to make sure the app is re-linked if the binary
# libraries change or are updated. If they change, the main esp32
# library will be rebuild by AR andthis will trigger a re-linking of
# the entire app.
#
# It would be better for components to be able to expose any of these
# non-standard dependencies via get_variable, but this will do for now.
$(COMPONENT_LIBRARY): $(ALL_LIB_FILES)

# Preprocess esp32.ld linker script into esp32_out.ld
#
# The library doesn't really depend on esp32_out.ld, but it
# saves us from having to add the target to a Makefile.projbuild
$(COMPONENT_LIBRARY): esp32_out.ld

esp32_out.ld: $(COMPONENT_PATH)/ld/esp32.ld ../include/sdkconfig.h
	$(CC) -I ../include -C -P -x c -E $< -o $@

COMPONENT_EXTRA_CLEAN := esp32_out.ld
