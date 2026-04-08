idf_component_register(SRC_DIRS "."
                    PRIV_INCLUDE_DIRS "."
                    PRIV_REQUIRES cmock mbedtls nvs_flash protocomm protobuf-c test_utils unity
                    WHOLE_ARCHIVE)
