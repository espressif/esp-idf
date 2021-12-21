idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "esp32c3")
    return()
endif()
if(BOOTLOADER_BUILD)
    # bootloader only needs headers from this component
    set(priv_requires soc)
else()
    set(priv_requires soc freertos)
    set(srcs "debug_helpers.c"
             "debug_helpers_asm.S"
             "expression_with_stack_xtensa.c"
             "expression_with_stack_xtensa_asm.S"
             "eri.c"
             "trax.c"
             "xtensa_intr.c"
             "xtensa_intr_asm.S"
             "${target}/trax_init.c"
             )

endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS include ${target}/include
                    LDFRAGMENTS linker.lf
                    PRIV_REQUIRES ${priv_requires})

if(NOT BOOTLOADER_BUILD)
    target_link_libraries(${COMPONENT_LIB} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/${target}/libxt_hal.a")
endif()
