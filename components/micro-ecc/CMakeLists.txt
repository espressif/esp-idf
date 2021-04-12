# only compile the "uECC_verify_antifault.c" file which includes the "micro-ecc/uECC.c" source file
set(COMPONENT_SRCS "uECC_verify_antifault.c")

set(COMPONENT_ADD_INCLUDEDIRS . micro-ecc)

set(COMPONENT_REQUIRES)

register_component()
