idf_build_get_property(target IDF_TARGET)
if(NOT "${target}" STREQUAL "esp32c3")
    return()
endif()
if(BOOTLOADER_BUILD)
    set(priv_requires soc)
else()
    set(priv_requires soc freertos)
    set(srcs
        "expression_with_stack_riscv.c"
        "expression_with_stack_riscv_asm.S"
        "instruction_decode.c"
        "interrupt.c"
        "stdatomic.c"
        "vectors.S")
endif()

idf_component_register(SRCS "${srcs}"
                       LDFRAGMENTS linker.lf
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES ${priv_requires})
