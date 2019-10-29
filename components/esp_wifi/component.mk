#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := include $(IDF_TARGET)/include
COMPONENT_SRCDIRS := src

ifndef CONFIG_ESP32_NO_BLOBS
    LIBS := core rtc net80211 pp smartconfig coexist espnow phy mesh
    COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/lib/$(IDF_TARGET) \
                          $(addprefix -l,$(LIBS))

    COMPONENT_ADD_LDFRAGMENTS += linker.lf

    COMPONENT_SUBMODULES += lib

    ALL_LIB_FILES := $(patsubst %,$(COMPONENT_PATH)/lib/$(IDF_TARGET)/lib%.a,$(LIBS))
    COMPONENT_ADD_LINKER_DEPS += $(ALL_LIB_FILES)
endif

