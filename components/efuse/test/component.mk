#
#Component Makefile
#
COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_SRCDIRS := . ${IDF_TARGET}
COMPONENT_PRIV_INCLUDEDIRS = "../private_include"

COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive
