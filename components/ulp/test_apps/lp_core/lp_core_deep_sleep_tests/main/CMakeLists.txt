set(app_sources "test_app_main.c" "lp_sleep_common.c")
set(lp_core_test_apps "")

if(CONFIG_SOC_ULP_LP_UART_SUPPORTED)
    list(APPEND app_sources "lp_sleep_regs.c" "test_lp_uart_sleep.c")
endif()

if(CONFIG_SOC_RTCIO_PIN_COUNT GREATER 0)
    list(APPEND app_sources "test_lp_gpio_sleep.c")
endif()

idf_component_register(SRCS ${app_sources}
                       INCLUDE_DIRS "lp_core"
                       REQUIRES ulp unity test_utils esp_hw_support
                       PRIV_REQUIRES esp_driver_gpio esp_driver_uart hal
                       WHOLE_ARCHIVE)

set(lp_core_exp_dep_srcs ${app_sources})

function(lp_core_test_app_checks)
    idf_build_get_property(python PYTHON)
    set(check_script "${CMAKE_CURRENT_LIST_DIR}/../../lp_core_basic_tests/check_lp_core_no_soft_float.py")

    foreach(app_name ${ARGN})
        set(symbol_file ${CMAKE_CURRENT_BINARY_DIR}/${app_name}/${app_name}.sym)
        set(check_output ${CMAKE_CURRENT_BINARY_DIR}/${app_name}/${app_name}.no_soft_float)
        add_custom_command(OUTPUT ${check_output}
                           COMMAND ${python} ${check_script} ${symbol_file}
                           COMMAND ${CMAKE_COMMAND} -E touch ${check_output}
                           DEPENDS ${symbol_file} ${check_script}
                           VERBATIM)
        add_custom_target(${app_name}_no_soft_float DEPENDS ${check_output})
        add_dependencies(${COMPONENT_LIB} ${app_name}_no_soft_float)
    endforeach()
endfunction()

if(CONFIG_SOC_ULP_LP_UART_SUPPORTED)
    ulp_embed_binary(lp_core_test_app_uart_sleep "lp_core/test_main_uart_sleep.c" "${lp_core_exp_dep_srcs}")
    ulp_embed_binary(lp_core_test_app_uart_wkup_sleep "lp_core/test_main_uart_wkup_sleep.c" "${lp_core_exp_dep_srcs}")
    list(APPEND lp_core_test_apps lp_core_test_app_uart_sleep lp_core_test_app_uart_wkup_sleep)
endif()

if(CONFIG_SOC_RTCIO_PIN_COUNT GREATER 0)
    ulp_embed_binary(lp_core_test_app_gpio_io_sleep "lp_core/test_main_gpio_io_sleep.c" "${lp_core_exp_dep_srcs}")
    ulp_embed_binary(lp_core_test_app_gpio_isr_sleep "lp_core/test_main_gpio_isr_sleep.c" "${lp_core_exp_dep_srcs}")
    list(APPEND lp_core_test_apps lp_core_test_app_gpio_io_sleep lp_core_test_app_gpio_isr_sleep)
    if(CONFIG_SOC_RTC_GPIO_EDGE_WAKEUP_SUPPORTED)
        ulp_embed_binary(lp_core_test_app_gpio_wkup_sleep
                         "lp_core/test_main_gpio_wkup_sleep.c"
                         "${lp_core_exp_dep_srcs}")
        list(APPEND lp_core_test_apps lp_core_test_app_gpio_wkup_sleep)
    endif()
endif()

lp_core_test_app_checks(${lp_core_test_apps})
