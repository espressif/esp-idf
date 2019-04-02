#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_SRCDIRS := src

LIBS ?=
ifndef CONFIG_NO_BLOBS
LIBS += core rtc net80211 pp wpa smartconfig coexist wps wpa2 espnow phy mesh
endif

COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/lib_esp32 \
                         $(addprefix -l,$(LIBS)) \

COMPONENT_ADD_LDFRAGMENTS += linker.lf

COMPONENT_SUBMODULES += lib_esp32

ALL_LIB_FILES := $(patsubst %,$(COMPONENT_PATH)/lib_esp32/lib%.a,$(LIBS))
COMPONENT_ADD_LINKER_DEPS += $(ALL_LIB_FILES)
