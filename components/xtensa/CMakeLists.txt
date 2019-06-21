idf_build_get_property(target IDF_TARGET)
idf_component_register(SRCS "debug_helpers.c" 
                            "debug_helpers_asm.S"
                            "eri.c" 
                            "trax.c" 
                    INCLUDE_DIRS include ${target}/include
                    LDFRAGMENTS linker.lf
                    PRIV_REQUIRES soc)

target_link_libraries(${COMPONENT_LIB} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/${target}/libhal.a")
