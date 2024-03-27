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
         "src/nvs_platform.cpp")

idf_component_register(SRCS "${srcs}"
                    REQUIRES "esp_partition"
                    PRIV_REQUIRES spi_flash newlib
                    INCLUDE_DIRS "include"
                    "../spi_flash/include"
                    PRIV_INCLUDE_DIRS "private_include")

# If we use the linux target, we need to redirect the crc functions to the linux
if(${target} STREQUAL "linux")
    if(CONFIG_NVS_ENCRYPTION)
        # mbedtls isn't configured for building with linux or as mock target. It will draw in all kind of dependencies
        message(FATAL_ERROR "NVS currently doesn't support encryption if built for Linux.")
    endif()
    target_compile_options(${COMPONENT_LIB} PUBLIC "-DLINUX_TARGET")
    target_compile_options(${COMPONENT_LIB} PUBLIC --coverage)
    target_link_libraries(${COMPONENT_LIB} PUBLIC --coverage)
else()
    target_sources(${COMPONENT_LIB} PRIVATE "src/nvs_encrypted_partition.cpp")
    target_link_libraries(${COMPONENT_LIB} PRIVATE idf::mbedtls)
endif()
