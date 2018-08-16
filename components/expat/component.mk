#
# Component Makefile
#
COMPONENT_ADD_INCLUDEDIRS := port/include include/expat

COMPONENT_SRCDIRS := library port

CFLAGS += -Wno-unused-function -DHAVE_EXPAT_CONFIG_H
# Temporary suppress "fallthrough" warnings until they are fixed in expat repo
CFLAGS += -Wno-implicit-fallthrough
