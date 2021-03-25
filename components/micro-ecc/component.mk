# only compile the micro-ecc/uECC.c source file
# (SRCDIRS is needed so build system can find the source file)
COMPONENT_SRCDIRS := .

COMPONENT_ADD_INCLUDEDIRS := . micro-ecc

COMPONENT_SUBMODULES := micro-ecc
