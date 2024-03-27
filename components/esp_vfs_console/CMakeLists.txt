idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs "vfs_console.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS include
                       PRIV_REQUIRES vfs esp_driver_uart esp_driver_usb_serial_jtag
                      )

if(CONFIG_ESP_CONSOLE_USB_CDC)
    target_sources(${COMPONENT_LIB} PRIVATE "vfs_cdcacm.c")
endif()

if(CONFIG_VFS_SUPPORT_IO)
    target_link_libraries(${COMPONENT_LIB} PUBLIC idf::vfs)
    # Make sure esp_vfs_console_register gets called at startup stage
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_vfs_include_console_register")
endif()
