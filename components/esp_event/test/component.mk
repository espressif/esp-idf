#
#Component Makefile
#

COMPONENT_PRIV_INCLUDEDIRS := ../private_include .
COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive