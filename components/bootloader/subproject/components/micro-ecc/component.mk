# only compile the "uECC_verify_antifault.c" file which includes the "micro-ecc/uECC.c" source file
COMPONENT_SRCDIRS := .

COMPONENT_ADD_INCLUDEDIRS := . micro-ecc

COMPONENT_SUBMODULES := micro-ecc
