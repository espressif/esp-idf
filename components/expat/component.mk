#
# Component Makefile
#
COMPONENT_ADD_INCLUDEDIRS := port/include include/expat

COMPONENT_SRCDIRS := library port

CFLAGS += -Wno-unused-function -DHAVE_EXPAT_CONFIG_H

