# only compile the micro-ecc/uECC.c source file
set(COMPONENT_SRCS micro-ecc/uECC.c)

set(COMPONENT_ADD_INCLUDEDIRS micro-ecc)

set(COMPONENT_REQUIRES)

register_component()
