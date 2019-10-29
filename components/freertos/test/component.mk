#
#Component Makefile
#

COMPONENT_SRCDIRS += esp32
COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive
