idf_build_get_property(target IDF_TARGET)

set(srcs)
set(public_include "include")

# TODO(IDF-15946): potentially remove __ULP_BUILD handling from esp_driver_uart
if(__ULP_BUILD)
    # LP-core UART wakeup shared code includes driver/uart_wakeup.h from
    # ulp/lp_core/shared/include/ulp_lp_core_lp_uart_shared.h.
    set(ulp_srcs)
    if(ULP_TYPE STREQUAL "lp_core" AND
            CONFIG_SOC_UART_SUPPORTED AND
            (CONFIG_SOC_DEEP_SLEEP_SUPPORTED OR CONFIG_SOC_LIGHT_SLEEP_SUPPORTED))
        list(APPEND ulp_srcs "src/uart_wakeup.c")
    endif()

    idf_component_register(SRCS ${ulp_srcs}
                           INCLUDE_DIRS ${public_include}
                           REQUIRES esp_common esp_hal_uart esp_hw_support log soc)
    return()
endif()

if(CONFIG_SOC_UART_SUPPORTED)
    list(APPEND srcs "src/uart.c")

    if(CONFIG_SOC_DEEP_SLEEP_SUPPORTED OR CONFIG_SOC_LIGHT_SLEEP_SUPPORTED)
        list(APPEND srcs "src/uart_wakeup.c")
    endif()

    if(CONFIG_SOC_UHCI_SUPPORTED)
        list(APPEND srcs "src/uhci.c")
    endif()
endif()

if(${target} STREQUAL "linux")
    set(priv_requires esp_ringbuf)
else()
    set(priv_requires esp_pm esp_driver_gpio esp_driver_dma esp_ringbuf esp_mm esp_psram)
endif()

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS ${public_include}
    REQUIRES esp_hal_uart
    PRIV_REQUIRES "${priv_requires}"
    LDFRAGMENTS "linker.lf"
)

if(CONFIG_VFS_SUPPORT_IO AND CONFIG_SOC_UART_SUPPORTED)
    target_link_libraries(${COMPONENT_LIB} PUBLIC idf::vfs)
    target_sources(${COMPONENT_LIB} PRIVATE "src/uart_vfs.c")
    if(CONFIG_ESP_CONSOLE_UART)
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-u uart_vfs_include_dev_init")
    endif()
endif()
