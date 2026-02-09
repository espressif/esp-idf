idf_build_get_property(target IDF_TARGET)

set(srcs)
set(includes "include")

# Add target-specific include directory if it exists
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
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
