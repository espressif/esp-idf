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
         "src/nvs_types.cpp")

set(public_req spi_flash)

set(include_dirs "include")

idf_component_register(SRCS "${srcs}"
                    REQUIRES "${public_req}"
                    INCLUDE_DIRS "${include_dirs}")

# If we use the linux target, we need to redirect the crc functions to the linux
if(${target} STREQUAL "linux")
    if(CONFIG_NVS_ENCRYPTION)
        # mbedtls isn't configured for building with linux or as mock target. It will draw in all kind of dependencies
        message(FATAL_ERROR "NVS currently doesn't support encryption if built for Linux.")
    endif()
    idf_component_get_property(spi_flash_dir spi_flash COMPONENT_DIR)
    target_include_directories(${COMPONENT_LIB} PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/mock/int"
            "${spi_flash_dir}/sim/stubs/freertos/include")
    target_sources(${COMPONENT_LIB} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/mock/int/crc.cpp")
    target_compile_options(${COMPONENT_LIB} PUBLIC "-DLINUX_TARGET")
else()
    # TODO: this is a workaround until IDF-2085 is fixed
    idf_component_get_property(mbedtls_lib mbedtls COMPONENT_LIB)
    target_link_libraries(${COMPONENT_LIB} PUBLIC ${mbedtls_lib})
endif()

if(CONFIG_NVS_ENCRYPTION)
    target_sources(${COMPONENT_LIB} PRIVATE "src/nvs_encrypted_partition.cpp")
    idf_component_get_property(mbedtls_lib mbedtls COMPONENT_LIB)
    target_link_libraries(${COMPONENT_LIB} PUBLIC ${mbedtls_lib})
endif()

if(${target} STREQUAL "linux")
    target_compile_options(${COMPONENT_LIB} PUBLIC --coverage)
    target_link_libraries(${COMPONENT_LIB} PUBLIC --coverage)
endif()
