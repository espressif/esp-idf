idf_component_register(SRCS "src/nvs_api.cpp"
                            "src/nvs_encr.cpp"
                            "src/nvs_item_hash_list.cpp"
                            "src/nvs_ops.cpp"
                            "src/nvs_page.cpp"
                            "src/nvs_pagemanager.cpp"
                            "src/nvs_storage.cpp"
                            "src/nvs_types.cpp"
                    REQUIRES spi_flash mbedtls
                    INCLUDE_DIRS include)
