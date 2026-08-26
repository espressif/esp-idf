idf_build_get_property(target IDF_TARGET)

set(srcs)
set(includes "include")

# Add target-specific include directory if it exists
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()

# TODO(IDF-15990): potentially remove __ULP_BUILDV2 handling from esp_hal_uart
if(__ULP_BUILDV2)
    # LP-core UART and print code includes UART HAL headers from
    # ulp/lp_core/lp_core/lp_core_uart.c and
    # ulp/lp_core/lp_core/lp_core_print.c.
    set(ulp_srcs)
    if(ULP_TYPE STREQUAL "lp_core" AND CONFIG_SOC_UART_SUPPORTED)
        list(APPEND ulp_srcs "uart_hal.c" "uart_hal_iram.c")
    endif()

    idf_component_register(SRCS ${ulp_srcs}
                           INCLUDE_DIRS ${includes}
                           REQUIRES esp_common soc hal)
    return()
endif()

if(CONFIG_SOC_UART_SUPPORTED)
    list(APPEND srcs "uart_hal.c" "uart_hal_iram.c" "${target}/uart_periph.c")
endif()

# UHCI HAL source
if(CONFIG_SOC_UHCI_SUPPORTED)
    list(APPEND srcs "uhci_hal.c")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${includes}
                    REQUIRES soc hal)
