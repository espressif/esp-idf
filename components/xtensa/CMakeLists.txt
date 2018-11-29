set(COMPONENT_SRCS "eri.c" "trax.c" "debug_helpers.c" "debug_helpers_asm.S")

set(COMPONENT_ADD_INCLUDEDIRS "include" "${IDF_TARGET}/include")

set(COMPONENT_ADD_LDFRAGMENTS linker.lf)
set(COMPONENT_PRIV_REQUIRES soc)

register_component()

target_link_libraries(${COMPONENT_LIB} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/${IDF_TARGET}/libhal.a")
