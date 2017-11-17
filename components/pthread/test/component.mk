#
#Component Makefile
#

COMPONENT_SRCDIRS := .

#CXXFLAGS += -H

COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive
