idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

if(BOOTLOADER_BUILD)
    # bootloader build simplified version
    set(srcs "src/nvs_bootloader.c"
             "src/nvs_bootloader_aes.c"
             "src/nvs_bootloader_xts_aes.c")

    set(requires "esp_partition")
    set(priv_requires "mbedtls" "nvs_sec_provider")

    idf_component_register(SRCS "${srcs}"
                        REQUIRES "${requires}"
                        PRIV_REQUIRES "${priv_requires}"
                        INCLUDE_DIRS "include"
                        PRIV_INCLUDE_DIRS "private_include"
    )

elseif(esp_tee_build)
    # esp-tee build simplified version
    set(srcs "src/nvs_api.cpp"
             "src/nvs_item_hash_list.cpp"
             "src/nvs_page.cpp"
             "src/nvs_pagemanager.cpp"
             "src/nvs_storage.cpp"
             "src/nvs_handle_simple.cpp"
             "src/nvs_handle_locked.cpp"
             "src/nvs_partition.cpp"
             "src/nvs_encrypted_partition.cpp"
             "src/nvs_partition_lookup.cpp"
             "src/nvs_partition_manager.cpp"
             "src/nvs_types.cpp"
             "src/nvs_platform.cpp")

    set(requires esp_partition esp_blockdev mbedtls)
    set(priv_requires spi_flash esp_libc cxx)

    idf_component_register(SRCS "${srcs}"
                    REQUIRES "${requires}"
                    PRIV_REQUIRES "${priv_requires}"
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS "private_include")

else()
    # regular, OS build
    idf_build_get_property(target IDF_TARGET)

    set(srcs "src/nvs_api.cpp"
            "src/nvs_cxx_api.cpp"
            "src/nvs_item_hash_list.cpp"
            "src/nvs_page.cpp"
            "src/nvs_pagemanager.cpp"
            "src/nvs_storage.cpp"
            "src/nvs_handle_simple.cpp"
            "src/nvs_handle_locked.cpp"
            "src/nvs_partition.cpp"
            "src/nvs_partition_lookup.cpp"
            "src/nvs_partition_manager.cpp"
            "src/nvs_types.cpp"
            "src/nvs_platform.cpp"
            "src/nvs_bootloader.c")

    set(requires esp_partition esp_blockdev)
    set(priv_requires spi_flash)
    if(NOT ${target} STREQUAL "linux")
        list(APPEND priv_requires esp_libc esptool_py nvs_sec_provider)
    endif()

    idf_component_register(SRCS "${srcs}"
                        REQUIRES "${requires}"
                        PRIV_REQUIRES "${priv_requires}"
                        INCLUDE_DIRS "include"
                        PRIV_INCLUDE_DIRS "private_include")

    # If we use the linux target, we need to redirect the crc functions to the linux
    if(${target} STREQUAL "linux")
        if(CONFIG_NVS_ENCRYPTION)
            # mbedtls isn't configured for building with linux or as mock target.
            # It will draw in all kind of dependencies
            message(FATAL_ERROR "NVS currently doesn't support encryption if built for Linux.")
        endif()
        target_compile_options(${COMPONENT_LIB} PUBLIC "-DLINUX_TARGET")
        target_compile_options(${COMPONENT_LIB} PUBLIC --coverage)
        target_link_libraries(${COMPONENT_LIB} PUBLIC --coverage)
    else()
        target_sources(${COMPONENT_LIB} PRIVATE "src/nvs_encrypted_partition.cpp"
                                                "src/nvs_bootloader_aes.c"
                                                "src/nvs_bootloader_xts_aes.c")
        target_link_libraries(${COMPONENT_LIB} PRIVATE idf::mbedtls)
    endif()

    if(CONFIG_COMPILER_STATIC_ANALYZER AND CMAKE_C_COMPILER_ID STREQUAL "GNU") # TODO IDF-10088
        target_compile_options(${COMPONENT_LIB} PUBLIC "-fno-analyzer")
    endif()

endif() #non-OS build
