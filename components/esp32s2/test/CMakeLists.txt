if(IDF_TARGET STREQUAL "esp32s2")
    idf_component_register(SRC_DIRS .
                        PRIV_INCLUDE_DIRS .
                        PRIV_REQUIRES unity test_utils nvs_flash ulp esp_common
                        )
endif()

