idf_build_get_property(target IDF_TARGET)

set(srcs)

if(CONFIG_ESP_CONSOLE_USB_CDC)
    list(APPEND srcs "usb_console.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS include
                       PRIV_REQUIRES esp_timer
                       LDFRAGMENTS linker.lf
                      )

if(CONFIG_VFS_SUPPORT_IO AND CONFIG_ESP_CONSOLE_USB_CDC)
    target_link_libraries(${COMPONENT_LIB} PUBLIC idf::vfs)
    target_sources(${COMPONENT_LIB} PRIVATE "vfs_cdcacm.c")
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_vfs_dev_cdcacm_include_dev_init")
endif()
