#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := include/
COMPONENT_SRCDIRS := src/

ifdef CONFIG_JSMN_PARENT_LINKS
src/jsmn.o: CPPFLAGS += -DJSMN_PARENT_LINKS
endif

ifdef CONFIG_JSMN_STRICT
src/jsmn.o: CPPFLAGS += -DJSMN_STRICT
endif
