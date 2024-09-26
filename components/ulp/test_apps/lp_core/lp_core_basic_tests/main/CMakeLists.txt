set(app_sources "test_app_main.c" "test_lp_core.c")

if(CONFIG_SOC_LP_I2C_SUPPORTED)
    list(APPEND app_sources "test_lp_core_i2c.c")
endif()

if(CONFIG_SOC_ULP_LP_UART_SUPPORTED)
    list(APPEND app_sources "test_lp_core_uart.c")
endif()

if(CONFIG_SOC_LP_SPI_SUPPORTED)
    list(APPEND app_sources "test_lp_core_spi.c")
endif()

if(CONFIG_SOC_LP_CORE_SUPPORT_ETM AND CONFIG_SOC_ETM_SUPPORTED)
    list(APPEND app_sources "test_lp_core_etm.c")
endif()

if(CONFIG_SOC_LP_ADC_SUPPORTED)
    list(APPEND app_sources "test_lp_core_adc.c")
endif()

if(CONFIG_SOC_LP_VAD_SUPPORTED)
    list(APPEND app_sources "test_lp_core_vad.c")
endif()

set(lp_core_sources         "lp_core/test_main.c")
set(lp_core_sources_counter "lp_core/test_main_counter.c")

if(CONFIG_SOC_LP_TIMER_SUPPORTED)
    set(lp_core_sources_set_timer_wakeup "lp_core/test_main_set_timer_wakeup.c")
endif()

set(lp_core_sources_gpio "lp_core/test_main_gpio.c")

if(CONFIG_SOC_LP_I2C_SUPPORTED)
    set(lp_core_sources_i2c "lp_core/test_main_i2c.c")
endif()

if(CONFIG_SOC_ULP_LP_UART_SUPPORTED)
    set(lp_core_sources_uart "lp_core/test_main_uart.c")
endif()

if(CONFIG_SOC_LP_SPI_SUPPORTED)
    set(lp_core_sources_spi_master "lp_core/test_main_spi_master.c")
    set(lp_core_sources_spi_slave "lp_core/test_main_spi_slave.c")
endif()

if(CONFIG_SOC_LP_ADC_SUPPORTED)
    set(lp_core_sources_adc "lp_core/test_main_adc.c")
endif()

if(CONFIG_SOC_LP_VAD_SUPPORTED)
    set(lp_core_sources_vad "lp_core/test_main_vad.c")
endif()

idf_component_register(SRCS ${app_sources}
                       INCLUDE_DIRS "lp_core"
                       REQUIRES ulp unity esp_timer test_utils
                       WHOLE_ARCHIVE
                       EMBED_FILES "test_vad_8k.pcm")

set(lp_core_exp_dep_srcs ${app_sources})

ulp_embed_binary(lp_core_test_app "${lp_core_sources}" "${lp_core_exp_dep_srcs}")
ulp_embed_binary(lp_core_test_app_counter "${lp_core_sources_counter}" "${lp_core_exp_dep_srcs}")
ulp_embed_binary(lp_core_test_app_isr "lp_core/test_main_isr.c"  "${lp_core_exp_dep_srcs}")

if(CONFIG_SOC_LP_TIMER_SUPPORTED)
    ulp_embed_binary(lp_core_test_app_set_timer_wakeup "${lp_core_sources_set_timer_wakeup}" "${lp_core_exp_dep_srcs}")
endif()

ulp_embed_binary(lp_core_test_app_gpio "${lp_core_sources_gpio}" "${lp_core_exp_dep_srcs}")

if(CONFIG_SOC_LP_I2C_SUPPORTED)
    ulp_embed_binary(lp_core_test_app_i2c "${lp_core_sources_i2c}" "${lp_core_exp_dep_srcs}")
endif()

if(CONFIG_SOC_ULP_LP_UART_SUPPORTED)
    ulp_embed_binary(lp_core_test_app_uart "${lp_core_sources_uart}" "${lp_core_exp_dep_srcs}")
endif()

if(CONFIG_SOC_LP_SPI_SUPPORTED)
    ulp_embed_binary(lp_core_test_app_spi_master "${lp_core_sources_spi_master}" "${lp_core_exp_dep_srcs}")
    ulp_embed_binary(lp_core_test_app_spi_slave "${lp_core_sources_spi_slave}" "${lp_core_exp_dep_srcs}")
endif()

if(CONFIG_SOC_LP_ADC_SUPPORTED)
    ulp_embed_binary(lp_core_test_app_adc "${lp_core_sources_adc}" "${lp_core_exp_dep_srcs}")
endif()

if(CONFIG_SOC_LP_VAD_SUPPORTED)
    ulp_embed_binary(lp_core_test_app_vad "${lp_core_sources_vad}" "${lp_core_exp_dep_srcs}")
endif()
