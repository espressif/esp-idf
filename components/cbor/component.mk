COMPONENT_SUBMODULES += tinycbor
COMPONENT_ADD_INCLUDEDIRS := port/include
COMPONENT_SRCDIRS :=  tinycbor/src
COMPONENT_PRIV_INCLUDEDIRS := tinycbor/src

tinycbor/src/open_memstream.o: CFLAGS += -D__GLIBC__
tinycbor/src/cbortojson.o: CFLAGS += -Wno-discarded-qualifiers
tinycbor/src/cborvalidation.o: CFLAGS += -Wno-maybe-uninitialized
