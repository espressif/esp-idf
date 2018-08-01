#
# Component Makefile
#
COMPONENT_ADD_INCLUDEDIRS := expat/expat/lib port/include

COMPONENT_SRCDIRS := expat/expat/lib port

CFLAGS += -DHAVE_EXPAT_CONFIG_H -DXML_POOR_ENTROPY
# Temporary suppress "fallthrough" warnings until they are fixed in expat repo
CFLAGS += -Wno-implicit-fallthrough

COMPONENT_SUBMODULES += expat
