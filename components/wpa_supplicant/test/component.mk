#
#Component Makefile
#

COMPONENT_PRIV_INCLUDEDIRS := ../src
COMPONENT_SRCDIRS := .

COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive

CFLAGS+= -DCONFIG_WPA3_SAE
