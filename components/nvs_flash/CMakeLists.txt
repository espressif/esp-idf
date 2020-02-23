set(srcs "src/nvs_api.cpp"
         "src/nvs_cxx_api.cpp"
         "src/nvs_item_hash_list.cpp"
         "src/nvs_ops.cpp"
         "src/nvs_page.cpp"
         "src/nvs_pagemanager.cpp"
         "src/nvs_storage.cpp"
         "src/nvs_handle_simple.cpp"
         "src/nvs_handle_locked.cpp"
         "src/nvs_partition_manager.cpp"
         "src/nvs_types.cpp")
if(CONFIG_NVS_ENCRYPTION)
    list(APPEND srcs "src/nvs_encr.cpp")
endif()

idf_component_register(SRCS "${srcs}"
                    REQUIRES spi_flash mbedtls
                    INCLUDE_DIRS include)
