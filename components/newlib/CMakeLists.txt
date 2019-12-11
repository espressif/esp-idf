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

if(CONFIG_SPIRAM_CACHE_WORKAROUND)
    set(ldfragments esp32-spiram-rom-functions-c.lf)
endif()

list(APPEND ldfragments newlib.lf)

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}"
                    REQUIRES vfs 
                    PRIV_REQUIRES soc
                    LDFRAGMENTS "${ldfragments}")  

# Toolchain libraries require code defined in this component
idf_component_get_property(newlib newlib COMPONENT_LIB)
target_link_libraries(${COMPONENT_LIB} INTERFACE ${LIBC} ${LIBM} gcc "$<TARGET_FILE:${newlib}>")

set_source_files_properties(heap.c PROPERTIES COMPILE_FLAGS -fno-builtin)

# Forces the linker to include locks, heap, and syscalls from this component,
# instead of the implementations provided by newlib.
set(EXTRA_LINK_FLAGS "-u newlib_include_locks_impl")
list(APPEND EXTRA_LINK_FLAGS "-u newlib_include_heap_impl")
list(APPEND EXTRA_LINK_FLAGS "-u newlib_include_syscalls_impl")
list(APPEND EXTRA_LINK_FLAGS "-u newlib_include_pthread_impl")
target_link_libraries(${COMPONENT_LIB} INTERFACE "${EXTRA_LINK_FLAGS}")
