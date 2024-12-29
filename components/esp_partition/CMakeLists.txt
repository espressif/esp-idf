# bootloader build simplified version
if(BOOTLOADER_BUILD)
set(srcs "partition_bootloader.c")
set(reqs "spi_flash")
set(priv_reqs "bootloader_support")

idf_component_register(SRCS "${srcs}"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS ${private_include_dirs}
    REQUIRES ${reqs}
    PRIV_REQUIRES ${priv_reqs})

# regular, non bootloader build
else()
set(srcs "partition.c")
set(priv_reqs esp_system spi_flash partition_table)
set(reqs)
set(private_include_dirs)

idf_build_get_property(build_dir BUILD_DIR)
idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    list(APPEND srcs "partition_linux.c")

    # Steal some include directories from bootloader_support components:
    idf_component_get_property(bootloader_support_dir bootloader_support COMPONENT_DIR)
    set(private_include_dirs ${bootloader_support_dir}/include)
else()
    list(APPEND priv_reqs bootloader_support app_update)
    list(APPEND srcs "partition_target.c")
endif()

idf_component_register(SRCS "${srcs}"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS ${private_include_dirs}
    REQUIRES ${reqs}
    PRIV_REQUIRES ${priv_reqs})

if(${target} STREQUAL "linux")
    # set BUILD_DIR because partition_linux.c uses a file created in the build directory
    target_compile_definitions(${COMPONENT_LIB} PRIVATE "BUILD_DIR=\"${build_dir}\"")
endif()

if(CMAKE_C_COMPILER_ID MATCHES "GNU")
    # These flags are GCC specific
    set_property(SOURCE ${cache_srcs} APPEND_STRING PROPERTY COMPILE_FLAGS
        " -fno-inline-small-functions -fno-inline-functions-called-once")
endif()

endif()
