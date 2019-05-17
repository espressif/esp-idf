set(COMPONENT_SRCS "heap.c"
                   "locks.c"
                   "poll.c"
                   "pthread.c"
                   "random.c"
                   "reent_init.c"
                   "select.c"
                   "syscall_table.c"
                   "syscalls.c"
                   "termios.c"
                   "time.c"
                   "utime.c")
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

    # Forces the linker to include locks, heap, and syscalls from this component,
    # instead of the implementations provided by newlib.
    set(EXTRA_LINK_FLAGS "-u newlib_include_locks_impl")
    list(APPEND EXTRA_LINK_FLAGS "-u newlib_include_heap_impl")
    list(APPEND EXTRA_LINK_FLAGS "-u newlib_include_syscalls_impl")

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
set(COMPONENT_PRIV_REQUIRES soc)

list(APPEND COMPONENT_ADD_LDFRAGMENTS newlib.lf)

register_component()

if (LIB_PATH)
    target_link_libraries(${COMPONENT_LIB} "-L ${LIB_PATH}")
endif()

if(GCC_NOT_5_2_0)
    # Toolchain libraries require code defined in this component
    add_library(extra INTERFACE)
    idf_component_get_property(newlib newlib COMPONENT_LIB)
    target_link_libraries(extra INTERFACE ${LIBC} ${LIBM} "$<TARGET_FILE:${newlib}>")
    target_link_libraries(${COMPONENT_LIB} extra)
else()
    target_link_libraries(${COMPONENT_LIB} ${LIBC} ${LIBM})
endif()

set_source_files_properties(heap.c PROPERTIES COMPILE_FLAGS -fno-builtin)

if(EXTRA_LINK_FLAGS)
    target_link_libraries(${COMPONENT_LIB} "${EXTRA_LINK_FLAGS}")
endif()
