idf_build_get_property(target IDF_TARGET)

# Bootloader builds only needs it for config, not for anything else
idf_build_get_property(non_os_build NON_OS_BUILD)

if(non_os_build OR ${target} STREQUAL "linux")
    idf_component_register()
    return()
endif()

set(srcs "stdio_vfs.c"
         "stdio_simple.c"
         "stdio_syscalls_simple.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS include)

if(CONFIG_VFS_SUPPORT_IO)
    # These drivers will be pulled in from the vfs driver
    # This maintains the old behavior of just having to add vfs as a REQUIRES to enable
    # the desired output driver. When we have requires that depend on kconfig values
    # this can be refactored to conditionally pull drivers into the build instead
    # TODO: IDF-13984 - Refactor to conditionally include stdio drivers based on Kconfig values
    idf_component_optional_requires(PRIVATE vfs esp_driver_uart esp_driver_usb_serial_jtag esp_usb_cdc_rom_console)

    target_link_libraries(${COMPONENT_LIB} PRIVATE idf::vfs)
    # Make sure esp_stdio_register gets called at startup stage
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_vfs_include_console_register")
endif()
