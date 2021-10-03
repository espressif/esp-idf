#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_SRCDIRS := src $(IDF_TARGET)

ifndef CONFIG_ESP32_NO_BLOBS
    LIBS := core net80211 pp smartconfig coexist espnow mesh
    COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/lib/$(IDF_TARGET) \
                          $(addprefix -l,$(LIBS))

    COMPONENT_ADD_LDFRAGMENTS += linker.lf

    COMPONENT_SUBMODULES += lib

    ALL_LIB_FILES := $(patsubst %,$(COMPONENT_PATH)/lib/$(IDF_TARGET)/lib%.a,$(LIBS))
    COMPONENT_ADD_LINKER_DEPS += $(ALL_LIB_FILES)
endif
