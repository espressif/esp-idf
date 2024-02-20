idf_build_get_property(target IDF_TARGET)
idf_build_get_property(arch IDF_TARGET_ARCH)

if(NOT "${arch}" STREQUAL "riscv")
    return()
endif()


if(BOOTLOADER_BUILD)
    set(priv_requires soc)
else()
    set(priv_requires soc)
    set(srcs
        "instruction_decode.c"
        "interrupt.c"
        "vectors.S")

    if(CONFIG_SOC_INT_CLIC_SUPPORTED)
        list(APPEND srcs "interrupt_clic.c" "vectors_clic.S")
    elseif(CONFIG_SOC_INT_PLIC_SUPPORTED)
        list(APPEND srcs "interrupt_plic.c" "vectors_intc.S")
    else()
        list(APPEND srcs "interrupt_intc.c" "vectors_intc.S")
    endif()
endif()

idf_component_register(SRCS "${srcs}"
                       LDFRAGMENTS linker.lf
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES ${priv_requires})

target_linker_script(${COMPONENT_LIB} INTERFACE "ld/rom.api.ld")
