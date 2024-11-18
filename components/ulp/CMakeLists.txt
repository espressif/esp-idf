idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs "")
set(includes "")

if(CONFIG_ULP_COPROC_ENABLED OR CONFIG_IDF_DOC_BUILD)
    list(APPEND includes
        ulp_common/include)
endif()

if(CONFIG_ULP_COPROC_TYPE_FSM OR (CONFIG_IDF_DOC_BUILD AND CONFIG_SOC_ULP_FSM_SUPPORTED))
    list(APPEND includes
        ulp_fsm/include
        ulp_fsm/include/${target})
endif()

if(CONFIG_ULP_COPROC_TYPE_RISCV OR CONFIG_IDF_DOC_BUILD)
    list(APPEND includes
        ulp_riscv/include
        ulp_riscv/shared/include)
endif()

if(CONFIG_ULP_COPROC_TYPE_LP_CORE OR CONFIG_IDF_DOC_BUILD)
    list(APPEND includes
         lp_core/include
         lp_core/shared/include)
endif()

if(CONFIG_ULP_COPROC_TYPE_FSM OR CONFIG_ULP_COPROC_TYPE_RISCV)

    list(APPEND srcs
        "ulp_common/ulp_common.c"
        "ulp_common/ulp_adc.c")

    if(CONFIG_ULP_COPROC_TYPE_FSM)
        list(APPEND srcs
            "ulp_fsm/ulp.c"
            "ulp_fsm/ulp_macro.c")

    elseif(CONFIG_ULP_COPROC_TYPE_RISCV)
        list(APPEND srcs
            "ulp_riscv/ulp_riscv.c"
            "ulp_riscv/ulp_riscv_lock.c"
            "ulp_riscv/ulp_riscv_i2c.c")
    endif()
endif()

if(CONFIG_ULP_COPROC_TYPE_LP_CORE)
    list(APPEND srcs
        "lp_core/lp_core.c"
        "lp_core/shared/ulp_lp_core_memory_shared.c"
        "lp_core/shared/ulp_lp_core_critical_section_shared.c")

    if(CONFIG_SOC_ULP_LP_UART_SUPPORTED)
        list(APPEND srcs "lp_core/lp_core_uart.c")
    endif()

    if(CONFIG_SOC_LP_I2C_SUPPORTED)
        list(APPEND srcs "lp_core/lp_core_i2c.c")
    endif()

    if(CONFIG_SOC_LP_TIMER_SUPPORTED)
        list(APPEND srcs "lp_core/shared/ulp_lp_core_lp_timer_shared.c")
    endif()

    if(CONFIG_SOC_LP_SPI_SUPPORTED)
        list(APPEND srcs "lp_core/lp_core_spi.c")
    endif()

    if(CONFIG_SOC_LP_CORE_SUPPORT_ETM)
        list(APPEND srcs "lp_core/lp_core_etm.c")
    endif()

    if(CONFIG_SOC_LP_ADC_SUPPORTED)
        list(APPEND srcs "lp_core/shared/ulp_lp_core_lp_adc_shared.c")
    endif()

    if(CONFIG_SOC_LP_VAD_SUPPORTED)
        list(APPEND srcs "lp_core/shared/ulp_lp_core_lp_vad_shared.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       REQUIRES driver esp_adc)
