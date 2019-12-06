set(srcs 
    "heap.c"
    "locks.c"
    "poll.c"
    "pread.c"
    "pwrite.c"
    "pthread.c"
    "random.c"
    "reent_init.c"
    "select.c"
    "syscall_table.c"
    "syscalls.c"
    "termios.c"
    "time.c"
    "utime.c")
set(include_dirs platform_include)

if(GCC_NOT_5_2_0)
    if(CONFIG_SPIRAM_CACHE_WORKAROUND)
        set(ldfragments esp32-spiram-rom-functions-c.lf)
    endif()

    # Forces the linker to include locks, heap, and syscalls from this component,
    # instead of the implementations provided by newlib.
    set(EXTRA_LINK_FLAGS "-u newlib_include_locks_impl")
    list(APPEND EXTRA_LINK_FLAGS "-u newlib_include_heap_impl")
    list(APPEND EXTRA_LINK_FLAGS "-u newlib_include_syscalls_impl")
    list(APPEND EXTRA_LINK_FLAGS "-u newlib_include_pthread_impl")
else()
    # Remove this section when GCC 5.2.0 is no longer supported
    # 'include' and 'lib' directories should also be removed.
    # An if statement about LIB_PATH below should also be removed.
    list(APPEND include_dirs include)
    set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

    if(CONFIG_SPIRAM_CACHE_WORKAROUND)
        set(ldfragments esp32-spiram-rom-functions-psram-workaround.lf)
    endif()
endif()

list(APPEND ldfragments newlib.lf)

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}"
                    REQUIRES vfs 
                    PRIV_REQUIRES soc
                    LDFRAGMENTS "${ldfragments}")  

if(LIB_PATH)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-L ${LIB_PATH}")
endif()

if(GCC_NOT_5_2_0)
    # Toolchain libraries require code defined in this component
    add_library(extra INTERFACE)
    idf_component_get_property(newlib newlib COMPONENT_LIB)
    target_link_libraries(extra INTERFACE ${LIBC} ${LIBM} gcc "$<TARGET_FILE:${newlib}>")
    target_link_libraries(${COMPONENT_LIB} PUBLIC extra)
else()
    target_link_libraries(${COMPONENT_LIB} PUBLIC ${LIBC} ${LIBM} gcc)
endif()

set_source_files_properties(heap.c PROPERTIES COMPILE_FLAGS -fno-builtin)

if(EXTRA_LINK_FLAGS)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "${EXTRA_LINK_FLAGS}")
endif()
