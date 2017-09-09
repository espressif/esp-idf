COMPONENT_ADD_INCLUDEDIRS := include port/include ../esp32/include
COMPONENT_SRCDIRS := src/crypto port src/fast_crypto

CFLAGS += -DEMBEDDED_SUPP -D__ets__ -Wno-strict-aliasing
