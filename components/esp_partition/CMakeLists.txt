set(srcs "partition.c")
set(priv_reqs esp_system bootloader_support spi_flash app_update partition_table)
set(reqs)
set(include_dirs "include")

idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    list(APPEND srcs "partition_linux.c")
    set(priv_reqs partition_table)

    # Steal some include directories from bootloader_support and hal components:
    idf_component_get_property(hal_dir hal COMPONENT_DIR)
    idf_component_get_property(bootloader_support_dir bootloader_support COMPONENT_DIR)
    list(APPEND include_dirs include ${hal_dir}/include ${bootloader_support_dir}/include)
else()
    list(APPEND srcs "partition_target.c")
endif()

idf_component_register(SRCS "${srcs}"
    INCLUDE_DIRS ${include_dirs}
    REQUIRES ${reqs}
    PRIV_REQUIRES ${priv_reqs})

if(CMAKE_C_COMPILER_ID MATCHES "GNU")
    # These flags are GCC specific
    set_property(SOURCE ${cache_srcs} APPEND_STRING PROPERTY COMPILE_FLAGS
        " -fno-inline-small-functions -fno-inline-functions-called-once")
endif()

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
