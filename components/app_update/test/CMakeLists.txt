if(IDF_TARGET STREQUAL "esp32")
    idf_component_register(SRC_DIRS "."
                        INCLUDE_DIRS "."
                        REQUIRES unity test_utils app_update bootloader_support nvs_flash)
endif()
