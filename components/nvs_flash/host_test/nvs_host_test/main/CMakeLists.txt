idf_component_register(SRCS "main.cpp"
                            "test_nvs.cpp"
                            "test_partition_manager.cpp"
                            "test_nvs_cxx_api.cpp"
                            "test_nvs_handle.cpp"
                            "test_nvs_initialization.cpp"
                            "test_nvs_storage.cpp"
                       INCLUDE_DIRS
                            "../../../src"
                            "../../../private_include"
                            "../../../../mbedtls/mbedtls/include"
                            "../../../../../tools/catch"
                       WHOLE_ARCHIVE
                       REQUIRES nvs_flash)
