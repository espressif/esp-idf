# only compile the micro-ecc/uECC.c source file
# (SRCDIRS is needed so build system can find the source file)
COMPONENT_SRCDIRS := micro-ecc
COMPONENT_OBJS := micro-ecc/uECC.o

COMPONENT_ADD_INCLUDEDIRS := micro-ecc

COMPONENT_SUBMODULES := micro-ecc
