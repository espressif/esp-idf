COMPONENT_ADD_INCLUDEDIRS := include port/include
COMPONENT_SRCDIRS := src/crypto port

CFLAGS += -DEMBEDDED_SUPP -D__ets__ -Wno-strict-aliasing
