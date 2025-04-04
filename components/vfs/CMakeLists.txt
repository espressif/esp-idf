idf_build_get_property(target IDF_TARGET)

# On Linux, we only support a few features, hence this simple component registration
if(${target} STREQUAL "linux")
    idf_component_register(SRCS "vfs_eventfd_linux.c"
                           INCLUDE_DIRS "include")
    return()
endif()

set(sources "")

list(APPEND sources "vfs.c"
                    "vfs_eventfd.c"
                    "vfs_semihost.c"
                    "nullfs.c"
                    )

list(APPEND pr esp_timer
               # for backwards compatibility (TODO: IDF-8799)
               esp_driver_uart esp_driver_usb_serial_jtag esp_vfs_console
               )

idf_component_register(SRCS ${sources}
                       LDFRAGMENTS "linker.lf"
                       INCLUDE_DIRS include
                       PRIV_INCLUDE_DIRS private_include
                       PRIV_REQUIRES ${pr})

# Some newlib syscalls are implemented in vfs.c, make sure these are always
# seen by the linker
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u vfs_include_syscalls_impl")

# Make sure nullfs is registered
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_vfs_include_nullfs_register")
