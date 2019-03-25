set(COMPONENT_SRCS "eri.c" "trax.c")

set(COMPONENT_ADD_INCLUDEDIRS "include" "${IDF_TARGET}/include")

set(COMPONENT_ADD_LDFRAGMENTS linker.lf)

register_component()
