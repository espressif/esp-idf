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
set(COMPONENT_ADD_INCLUDEDIRS platform_include include)

if(CONFIG_SPIRAM_CACHE_WORKAROUND)
    set(LIBC c-psram-workaround)
    set(LIBM m-psram-workaround)

    set(COMPONENT_ADD_LDFRAGMENTS esp32-spiram-rom-functions.lf)
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

target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib")
target_link_libraries(${COMPONENT_TARGET} ${LIBC} ${LIBM})

set_source_files_properties(syscalls.c PROPERTIES COMPILE_FLAGS -fno-builtin)
