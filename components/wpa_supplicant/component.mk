COMPONENT_ADD_INCLUDEDIRS := include port/include
COMPONENT_SRCDIRS := src/crypto

CFLAGS += -DEMBEDDED_SUPP -D__ets__

include $(IDF_PATH)/make/component_common.mk
