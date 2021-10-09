COMPONENT_SUBMODULES += tinycbor
COMPONENT_ADD_INCLUDEDIRS := port/include
COMPONENT_SRCDIRS :=  tinycbor/src
COMPONENT_PRIV_INCLUDEDIRS := tinycbor/src

tinycbor/src/open_memstream.o: CFLAGS += -D__linux__
