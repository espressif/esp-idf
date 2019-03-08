set(COMPONENT_SRCS "locks.c"
                   "pthread.c"
                   "random.c"
                   "reent_init.c"
                   "select.c"
                   "poll.c"
                   "syscall_table.c"
                   "syscalls.c"
                   "termios.c"
                   "utime.c"
                   "time.c")
set(COMPONENT_ADD_INCLUDEDIRS platform_include)


if(GCC_NOT_5_2_0)
    if(CONFIG_NEWLIB_NANO_FORMAT)
        set(LIBC c_nano)
    else()
        set(LIBC c)
    endif()

    set(LIBM m)

    if(CONFIG_SPIRAM_CACHE_WORKAROUND)
        set(COMPONENT_ADD_LDFRAGMENTS esp32-spiram-rom-functions-c.lf)
    endif()

else()
    # Remove this section when GCC 5.2.0 is no longer supported
    # 'include' and 'lib' directories should also be removed.
    # An if statement about LIB_PATH below should also be removed.
    list(APPEND COMPONENT_ADD_INCLUDEDIRS include)
    set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

    if(CONFIG_SPIRAM_CACHE_WORKAROUND)
        set(LIBC c-psram-workaround)
        set(LIBM m-psram-workaround)

        set(COMPONENT_ADD_LDFRAGMENTS esp32-spiram-rom-functions-psram-workaround.lf)
    else()

        if(CONFIG_NEWLIB_NANO_FORMAT)
            set(LIBC c_nano)
        else()
            set(LIBC c)
        endif()

        set(LIBM m)
    endif()
endif()
set(COMPONENT_REQUIRES vfs)  # for sys/ioctl.h

register_component()

if (LIB_PATH)
    target_link_libraries(${COMPONENT_TARGET} "-L ${LIB_PATH}")
endif()
target_link_libraries(${COMPONENT_TARGET} ${LIBC} ${LIBM})

set_source_files_properties(syscalls.c PROPERTIES COMPILE_FLAGS -fno-builtin)
