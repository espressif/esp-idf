idf_build_get_property(target IDF_TARGET)

# On Linux, we only support a few features, hence this simple component registration
if(${target} STREQUAL "linux")

    list(APPEND inc include)
    if(CONFIG_VFS_SUPPORT_IO)
        list(APPEND inc linux_include)
        list(APPEND priv_inc private_include)
        list(APPEND srcs "vfs_linux.c" "vfs.c")
    endif()

    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
        list(APPEND srcs "vfs_eventfd_linux.c")
    endif()

    idf_component_register(SRCS ${srcs}
                           LDFRAGMENTS "linker.lf"
                           INCLUDE_DIRS ${inc}
                           PRIV_INCLUDE_DIRS ${priv_inc})
    return()
endif()

set(sources "")

# These are here to pull the misc stdio drivers into the build when using VFS
# This maintains the old behavior of just having to add vfs as a REQUIRES to enable
# the desired output driver. For the new cmake v2 build system this is no longer
# necessary as we can conditionally pull in dependencies.

list(APPEND pr esp_driver_uart esp_driver_usb_serial_jtag esp_usb_cdc_rom_console)

if(IDF_BUILD_V2)
    # Clear driver dependencies in v2, these will be pulled in conditionally by stdio
    set(pr "")
endif()

list(APPEND sources "vfs.c"
                    "vfs_eventfd.c"
                    "vfs_semihost.c"
                    "nullfs.c"
                    )

idf_component_register(SRCS ${sources}
                       LDFRAGMENTS "linker.lf"
                       INCLUDE_DIRS include
                       PRIV_INCLUDE_DIRS private_include
                       PRIV_REQUIRES ${pr})

# Some libc syscalls are implemented in vfs.c, make sure these are always
# seen by the linker
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u vfs_include_syscalls_impl")

# Make sure nullfs is registered
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_vfs_include_nullfs_register")
