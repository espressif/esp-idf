COMPONENT_ADD_INCLUDEDIRS := .
COMPONENT_SRCDIRS := linenoise argtable3 .

argtable3/argtable3.o: CFLAGS += -Wno-clobbered
