COMPONENT_ADD_INCLUDEDIRS := include port/include
COMPONENT_SRCDIRS := src/crypto

CFLAGS += -DEMBEDDED_SUPP -D__ets__ -Wno-strict-aliasing -D__ESP32_SUPP_TYPE__
