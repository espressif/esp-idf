idf_build_get_property(target IDF_TARGET)
idf_build_get_property(non_os_build NON_OS_BUILD)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(include_dirs "platform_include")

if(non_os_build)
    # Bootloader builds need the platform_include directory (for assert.h), but nothing else
    idf_component_register(INCLUDE_DIRS platform_include)
    return()
endif()

set(srcs
    "src/init.c"
    "src/abort.c"
    "src/assert.c"
    "src/heap.c"
    "src/locks.c"
    "src/poll.c"
    "src/pthread.c"
    "src/random.c"
    "src/getentropy.c"
    "src/termios.c"
    "src/stdatomic.c"
    "src/time.c"
    "src/sysconf.c"
    "src/realpath.c"
    "src/scandir.c"
    "src/syscalls.c"
    "src/reent_syscalls.c"
    "src/port/esp_time_impl.c")

# TODO IDF-15041: remove this block and sources
if(CONFIG_LIBC_PICOLIBC AND CONFIG_SPIRAM_CACHE_WORKAROUND)
    list(APPEND srcs "src/picolibc/esp32_psram/memcpy.S")
    list(APPEND srcs "src/picolibc/esp32_psram/memset.S")
    list(APPEND srcs "src/picolibc/esp32_psram/strcpy.S")
    list(APPEND srcs "src/picolibc/esp32_psram/strncpy.S")
    list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_include_memcpy_impl")
    list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_include_memset_impl")
    list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_include_strcpy_impl")
    list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_include_strncpy_impl")
endif()

if(CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND)
    list(APPEND srcs "src/port/xtensa/stdatomic_s32c1i.c")
endif()

if(CONFIG_LIBC_OPTIMIZED_MISALIGNED_ACCESS)
    list(APPEND srcs
         "src/port/riscv/memcpy.c"
         "src/port/riscv/memmove.c"
         "src/string/memcmp.c"
         "src/port/riscv/strcpy.c"
         "src/string/strncpy.c"
         "src/string/strncmp.c"
         "src/port/riscv/strcmp.S")
    list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_include_memcpy_impl")
    list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_include_memmove_impl")
    list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_include_memcmp_impl")
    list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_include_strcpy_impl")
    list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_include_strncpy_impl")
    list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_include_strncmp_impl")
    list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_include_strcmp_impl")
endif()

if(CONFIG_LIBC_NEWLIB)
    list(APPEND srcs
        "src/flockfile.c"
        "src/reent_init.c"
        "src/newlib_init.c")
else()
    list(APPEND srcs
         "src/picolibc/picolibc_init.c"
         "src/picolibc/rand.c"
         "src/picolibc/open_memstream.c"
         "src/picolibc/errno.c")
    if(CONFIG_LIBC_PICOLIBC_NEWLIB_COMPATIBILITY)
        list(APPEND srcs "src/picolibc/getreent.c")
        list(APPEND srcs "src/picolibc/fopencookie.c")
    endif()
endif()

set(ldfragments "")
list(APPEND ldfragments "src/esp_libc.lf" "src/system_libs.lf")

if(CONFIG_LIBC_NEWLIB)
    list(APPEND ldfragments src/libc.lf)
    list(APPEND ldfragments src/libm.lf)
else()
    list(APPEND ldfragments src/picolibc/libc.lf)
endif()

set(priv_reqs soc spi_flash)

if(IDF_BUILD_V2)
    if(CONFIG_VFS_SUPPORT_IO)
        list(APPEND priv_reqs vfs)
    endif()
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS platform_include
                    PRIV_INCLUDE_DIRS priv_include
                    PRIV_REQUIRES "${priv_reqs}"
                    LDFRAGMENTS "${ldfragments}")

# Toolchain libraries require code defined in this component
idf_component_get_property(esp_libc esp_libc COMPONENT_LIB)
target_link_libraries(${COMPONENT_LIB} INTERFACE c m ${CONFIG_COMPILER_RT_LIB_NAME} "$<TARGET_FILE:${esp_libc}>")

set_source_files_properties(heap.c PROPERTIES COMPILE_FLAGS -fno-builtin)

# TODO IDF-15041: remove this block and sources
if(CONFIG_LIBC_PICOLIBC AND CONFIG_SPIRAM_CACHE_WORKAROUND)
    set_source_files_properties("src/picolibc/esp32_psram/memcpy.S"
                                "src/picolibc/esp32_psram/memset.S"
                                "src/picolibc/esp32_psram/strcpy.S"
                                "src/picolibc/esp32_psram/strncpy.S"
                                PROPERTIES COMPILE_FLAGS "-DXTENSA_ESP32_PSRAM_CACHE_FIX=1")
endif()

if(CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND)
    set_source_files_properties("src/port/xtensa/stdatomic_s32c1i.c"
                                PROPERTIES COMPILE_FLAGS "-mno-disable-hardware-atomics")
endif()

if(CONFIG_LIBC_OPTIMIZED_MISALIGNED_ACCESS)
    # TODO GCC-419 and IDF-13089: cleanup files
    set_source_files_properties("src/string/memcmp.c"
                                "src/string/strncmp.c"
                                "src/string/strncpy.c"
                                "src/port/riscv/memcpy.c"
                                "src/port/riscv/memmove.c"
                                "src/port/riscv/strcpy.c"
                                PROPERTIES COMPILE_FLAGS -O2)
endif()

# Forces the linker to include heap, syscall, pthread, assert, and retargetable locks from this component,
# instead of the implementations provided by newlib.
list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_include_heap_impl")
list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_include_reent_syscalls_impl")
list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_include_syscalls_impl")
list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_include_pthread_impl")
list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_include_assert_impl")
list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_include_getentropy_impl")
list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_include_init_funcs")
list(APPEND EXTRA_LINK_FLAGS "-u esp_libc_init_funcs")
target_link_libraries(${COMPONENT_LIB} INTERFACE "${EXTRA_LINK_FLAGS}")

if(CONFIG_LIBC_NEWLIB_NANO_FORMAT)
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
    endif()
endif()

add_subdirectory(src/port)

# if lwip is included in the build, add it as a public requirement so that
# #include <sys/socket.h> works without any special provisions.
idf_component_optional_requires(PUBLIC lwip)
