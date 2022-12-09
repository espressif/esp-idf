set(srcs 
    "abort.c"
    "heap.c"
    "locks.c"
    "poll.c"
    "pthread.c"
    "random.c"
    "reent_init.c"
    "syscall_table.c"
    "syscalls.c"
    "termios.c"
    "time.c")
set(include_dirs platform_include)

if(CONFIG_SPIRAM_CACHE_WORKAROUND)
    set(ldfragments esp32-spiram-rom-functions-c.lf)
endif()

list(APPEND ldfragments newlib.lf)

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}"
                    PRIV_REQUIRES soc esp_timer
                    LDFRAGMENTS "${ldfragments}")  

# Toolchain libraries require code defined in this component
idf_component_get_property(newlib newlib COMPONENT_LIB)
target_link_libraries(${COMPONENT_LIB} INTERFACE c m gcc "$<TARGET_FILE:${newlib}>")

set_source_files_properties(heap.c PROPERTIES COMPILE_FLAGS -fno-builtin)

# Forces the linker to include locks, heap, and syscalls from this component,
# instead of the implementations provided by newlib.
set(EXTRA_LINK_FLAGS "-u newlib_include_locks_impl")
list(APPEND EXTRA_LINK_FLAGS "-u newlib_include_heap_impl")
list(APPEND EXTRA_LINK_FLAGS "-u newlib_include_syscalls_impl")
list(APPEND EXTRA_LINK_FLAGS "-u newlib_include_pthread_impl")
target_link_libraries(${COMPONENT_LIB} INTERFACE "${EXTRA_LINK_FLAGS}")

if(CONFIG_NEWLIB_NANO_FORMAT)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "--specs=nano.specs")
endif()
