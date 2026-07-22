idf_build_get_property(target IDF_TARGET)
idf_build_get_property(arch IDF_TARGET_ARCH)
idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

# TODO(IDF-15955): potentially remove __ULP_BUILD handling from riscv
if(NOT "${arch}" STREQUAL "riscv" AND NOT (__ULP_BUILD AND "${ULP_TYPE}" STREQUAL "riscv"))
    return()
endif()

if(__ULP_BUILD AND "${ULP_TYPE}" STREQUAL "riscv")
    # RISC-V ULP interrupt support includes riscv/interrupt.h from
    # ulp/ulp_riscv/ulp_core/include/ulp_riscv_interrupt.h.
    idf_component_register(INCLUDE_DIRS "include")
    return()
endif()

# When __ULP_BUILD and ULP_TYPE is lp_core, keep the normal RISC-V helper sources for headers
# used by ulp/lp_core/lp_core/lp_core_pmp.c and vector.S.

if(BOOTLOADER_BUILD)
    set(priv_requires soc hal esp_hal_debug_assist)
    set(srcs "rv_utils.c")
elseif(esp_tee_build)
    set(priv_requires soc hal esp_hal_debug_assist)
    set(srcs "rv_utils.c")
    if(CONFIG_SOC_INT_PLIC_SUPPORTED)
        list(APPEND srcs "interrupt_plic.c")
    elseif(CONFIG_SOC_INT_CLIC_SUPPORTED)
        list(APPEND srcs "interrupt_clic.c")
    endif()
else()
    set(priv_requires soc hal esp_hal_debug_assist)
    set(srcs
        "instruction_decode.c"
        "interrupt.c"
        "rv_utils.c"
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
