#
#Component Makefile
#

COMPONENT_SRCDIRS += esp32
COMPONENT_SRCDIRS += param_test
COMPONENT_PRIV_INCLUDEDIRS += param_test/include

COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive
