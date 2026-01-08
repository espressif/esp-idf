idf_build_get_property(target IDF_TARGET)

set(srcs)
set(public_include "include")
if(CONFIG_SOC_UART_SUPPORTED)
    list(APPEND srcs "src/uart.c" "src/uart_wakeup.c")
    if(CONFIG_SOC_UHCI_SUPPORTED)
        list(APPEND srcs "src/uhci.c")
    endif()
endif()

if(${target} STREQUAL "linux")
    set(priv_requires esp_ringbuf)
else()
    set(priv_requires esp_pm esp_driver_gpio esp_ringbuf esp_mm esp_psram)
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
