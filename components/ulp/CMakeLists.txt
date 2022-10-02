idf_build_get_property(target IDF_TARGET)

set(srcs "")
set(includes "")

if(CONFIG_SOC_ULP_SUPPORTED OR CONFIG_SOC_RISCV_COPROC_SUPPORTED)

    list(APPEND srcs
        "ulp_common/ulp_common.c"
        "ulp_common/ulp_adc.c")

    list(APPEND includes
        ulp_common/include
        ulp_common/include/${target})

    if(CONFIG_ULP_COPROC_TYPE_FSM)
        list(APPEND srcs
            "ulp_fsm/ulp.c"
            "ulp_fsm/ulp_macro.c")

            list(APPEND includes
                ulp_fsm/include
                ulp_fsm/include/${target})

    elseif(CONFIG_ULP_COPROC_TYPE_RISCV)
        list(APPEND srcs
            "ulp_riscv/ulp_riscv.c"
            "ulp_riscv/ulp_riscv_lock.c"
            "ulp_riscv/ulp_riscv_i2c.c")

        list(APPEND includes
            ulp_riscv/include
            ulp_riscv/shared/include)
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       REQUIRES driver esp_adc)

if(CONFIG_SOC_ULP_SUPPORTED OR CONFIG_SOC_RISCV_COPROC_SUPPORTED)
    target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
endif()
