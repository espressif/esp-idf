idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

if(BOOTLOADER_BUILD)
    # Bootloader builds need the platform_include directory (for assert.h), but nothing else
    idf_component_register(INCLUDE_DIRS platform_include)
    return()
endif()

set(srcs
    "abort.c"
    "assert.c"
    "heap.c"
    "locks.c"
    "poll.c"
    "pthread.c"
    "random.c"
    "getentropy.c"
    "reent_init.c"
    "newlib_init.c"
    "syscalls.c"
    "termios.c"
    "stdatomic.c"
    "time.c"
    "sysconf.c"
    "realpath.c")
set(include_dirs platform_include)

if(CONFIG_SPIRAM_CACHE_WORKAROUND)
    set(ldfragments "esp32-spiram-rom-functions-c.lf")
endif()

list(APPEND ldfragments "newlib.lf" "system_libs.lf")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}"
                    PRIV_INCLUDE_DIRS priv_include
                    PRIV_REQUIRES soc spi_flash
                    LDFRAGMENTS "${ldfragments}")

# Toolchain libraries require code defined in this component
idf_component_get_property(newlib newlib COMPONENT_LIB)
target_link_libraries(${COMPONENT_LIB} INTERFACE c m ${CONFIG_COMPILER_RT_LIB_NAME} "$<TARGET_FILE:${newlib}>")

set_source_files_properties(heap.c PROPERTIES COMPILE_FLAGS -fno-builtin)

# Forces the linker to include heap, syscall, pthread, assert, and retargetable locks from this component,
# instead of the implementations provided by newlib.
list(APPEND EXTRA_LINK_FLAGS "-u newlib_include_heap_impl")
list(APPEND EXTRA_LINK_FLAGS "-u newlib_include_syscalls_impl")
list(APPEND EXTRA_LINK_FLAGS "-u newlib_include_pthread_impl")
list(APPEND EXTRA_LINK_FLAGS "-u newlib_include_assert_impl")
target_link_libraries(${COMPONENT_LIB} INTERFACE "${EXTRA_LINK_FLAGS}")

if(CONFIG_NEWLIB_NANO_FORMAT)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(libc_dir_cmd ${CMAKE_C_COMPILER})
        string(REPLACE " " ";" cflags_list ${CMAKE_C_FLAGS})
        list(APPEND libc_dir_cmd ${cflags_list} "-print-file-name=libc.a")
        execute_process(
            COMMAND ${libc_dir_cmd}
            OUTPUT_VARIABLE libc_dir
        )
        get_filename_component(libc_dir ${libc_dir} DIRECTORY)
        target_link_directories(${COMPONENT_LIB} INTERFACE "${libc_dir}/nano")
    else()
        target_link_libraries(${COMPONENT_LIB} INTERFACE "--specs=nano.specs")
    endif()
endif()

add_subdirectory(port)

# if lwip is included in the build, add it as a public requirement so that
# #include <sys/socket.h> works without any special provisions.
idf_component_optional_requires(PUBLIC lwip)
