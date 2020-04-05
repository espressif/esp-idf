idf_build_get_property(target IDF_TARGET)

if(BOOTLOADER_BUILD)
    # bootloader only needs headers from this component
    set(priv_requires soc)
else()
    set(priv_requires soc freertos)
    set(srcs "debug_helpers.c"
             "debug_helpers_asm.S"
             "expression_with_stack_xtensa_asm.S"
             "expression_with_stack_xtensa.c"
             "eri.c"
             "trax.c"
             "${target}/trax_init.c"
             )

    if(IDF_TARGET STREQUAL "esp32s2")
        list(APPEND srcs "stdatomic.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS include ${target}/include
                    LDFRAGMENTS linker.lf
                    PRIV_REQUIRES ${priv_requires})

if(NOT BOOTLOADER_BUILD)
    target_link_libraries(${COMPONENT_LIB} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/${target}/libhal.a")
endif()
