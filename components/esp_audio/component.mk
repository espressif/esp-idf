#
# Component Makefile
#
# This Makefile should, at the very least, just include $(IDF_PATH)/make/component_common.mk. By default, 
# this will take the sources in this directory, compile them and link them into 
# lib(subdirectory_name).a in the build directory. This behaviour is entirely configurable,
# please read the esp-idf build system document if you need to do this.
#
LIBS := esp_audio

COMPONENT_ADD_INCLUDEDIRS := include include/driver

COMPONENT_ADD_LDFLAGS :=  \
                           $(abspath lib/libEspAudio.a) \
                           $(abspath lib/libMisc.a) \
                           $(abspath lib/libMediaHal.a) \
                           $(abspath lib/libdlna.a) \

include $(IDF_PATH)/make/component_common.mk


# this is a hack to make sure the app is re-linked if the binary
# libraries change or are updated. If they change, the main esp32
# library will be rebuild by AR andthis will trigger a re-linking of
# the entire app.
#
# It would be better for components to be able to expose any of these
# non-standard dependencies via get_variable, but this will do for now.
$(COMPONENT_LIBRARY): $(ALL_LIB_FILES)
