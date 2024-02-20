idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

list(APPEND sources "vfs.c"
                    "vfs_eventfd.c"
                    "vfs_semihost.c"
                    "vfs_console.c"
                    )

list(APPEND pr esp_timer
               # for backwards compatibility (TODO: IDF-8799)
               esp_driver_uart esp_driver_usb_serial_jtag
               )

idf_component_register(SRCS ${sources}
                       LDFRAGMENTS "linker.lf"
                       INCLUDE_DIRS include
                       PRIV_INCLUDE_DIRS private_include
                       PRIV_REQUIRES ${pr})

if(CONFIG_ESP_CONSOLE_USB_CDC)
    target_sources(${COMPONENT_LIB} PRIVATE "vfs_cdcacm.c")
endif()

# Some newlib syscalls are implemented in vfs.c, make sure these are always
# seen by the linker
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u vfs_include_syscalls_impl")

if(CONFIG_VFS_SUPPORT_IO)
    # Make sure esp_vfs_console_register gets called at startup stage
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_vfs_include_console_register")
endif()
