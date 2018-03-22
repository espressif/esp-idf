set(COMPONENT_SRCDIRS ".")
set(COMPONENT_ADD_INCLUDEDIRS platform_include include)

if(CONFIG_SPIRAM_CACHE_WORKAROUND)
    set(LIBC c-psram-workaround)
    set(LIBM m-psram-workaround)
else()

    if(CONFIG_NEWLIB_NANO_FORMAT)
        set(LIBC c_nano)
    else()
        set(LIBC c)
    endif()

    set(LIBM m)
endif()

set(COMPONENT_REQUIRES vfs)  # for sys/ioctl.h

register_component()

target_link_libraries(newlib "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib")
target_link_libraries(newlib ${LIBC} ${LIBM})
