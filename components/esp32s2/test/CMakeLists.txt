if(IDF_TARGET STREQUAL "esp32s2")
    idf_component_register(SRC_DIRS .
                        PRIV_INCLUDE_DIRS .
                        PRIV_REQUIRES cmock test_utils nvs_flash mbedtls ulp esp_common
                        )
endif()
