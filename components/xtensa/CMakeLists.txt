idf_build_get_property(target IDF_TARGET)
idf_build_get_property(arch IDF_TARGET_ARCH)

if(NOT "${arch}" STREQUAL "xtensa")
    return()
endif()

set(srcs "eri.c" "xt_trax.c")

if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs "xtensa_intr.c" "xtensa_intr_asm.S")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS include ${target}/include
                    LDFRAGMENTS linker.lf)

target_link_libraries(${COMPONENT_LIB} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/${target}/libxt_hal.a")
