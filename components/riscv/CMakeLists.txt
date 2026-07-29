idf_build_get_property(target IDF_TARGET)
idf_build_get_property(arch IDF_TARGET_ARCH)
idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

# TODO(IDF-15955): potentially remove __ULP_BUILDV2 handling from riscv
if(NOT "${arch}" STREQUAL "riscv" AND NOT (__ULP_BUILDV2 AND "${ULP_TYPE}" STREQUAL "riscv"))
    return()
endif()

if(__ULP_BUILDV2 AND "${ULP_TYPE}" STREQUAL "riscv")
    # RISC-V ULP interrupt support includes riscv/interrupt.h from
    # ulp/ulp_riscv/ulp_core/include/ulp_riscv_interrupt.h.
    idf_component_register(INCLUDE_DIRS "include")
    return()
endif()

# When __ULP_BUILDV2 and ULP_TYPE is lp_core, keep the normal RISC-V helper sources for headers
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

# TODO: IDF-14145
if(CONFIG_COMPILER_ENABLE_RISCV_ZCMP AND
   NOT CONFIG_ESP32P4_SELECTS_REV_LESS_V3 AND
   NOT CONFIG_SECURE_ENABLE_TEE AND
   NOT ESP_TEE_BUILD)
    if(IDF_BUILD_V2)
        function(riscv_check_zcmp_workaround target)
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND}
                    -DELF_FILE=$<TARGET_FILE:${target}>
                    -DCMAKE_OBJDUMP=${CMAKE_OBJDUMP}
                    -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_zcmp_workaround.cmake"
                COMMENT "Checking ZCMP workaround in linked executable"
                VERBATIM)
        endfunction()
        idf_component_register_build_event_callback(EVENT POST_ELF CALLBACK riscv_check_zcmp_workaround)
    else()
        idf_build_get_property(build_dir BUILD_DIR)
        idf_build_get_property(elf_target EXECUTABLE GENERATOR_EXPRESSION)
        set(_riscv_zcmp_check_marker "${build_dir}/.zcmp_workaround_checked")
        add_custom_command(OUTPUT "${_riscv_zcmp_check_marker}"
            COMMAND ${CMAKE_COMMAND}
                -DELF_FILE=$<TARGET_FILE:$<GENEX_EVAL:${elf_target}>>
                -DCMAKE_OBJDUMP=${CMAKE_OBJDUMP}
                -P "${CMAKE_CURRENT_LIST_DIR}/check_zcmp_workaround.cmake"
            COMMAND ${CMAKE_COMMAND} -E touch "${_riscv_zcmp_check_marker}"
            DEPENDS "$<TARGET_FILE:$<GENEX_EVAL:${elf_target}>>"
            COMMENT "Checking ZCMP workaround in linked executable"
            VERBATIM)
        add_custom_target(riscv_check_zcmp_workaround DEPENDS "${_riscv_zcmp_check_marker}")
        idf_build_add_post_elf_dependency("${CMAKE_PROJECT_NAME}.elf" riscv_check_zcmp_workaround)
    endif()
endif()
