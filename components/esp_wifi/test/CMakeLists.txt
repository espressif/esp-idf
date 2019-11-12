set(srcdirs ".")
if(IDF_TARGET STREQUAL "esp32")
    list(APPEND srcdirs "esp32")
endif()

idf_component_register(SRC_DIRS ${srcdirs}
                       INCLUDE_DIRS . ${CMAKE_CURRENT_BINARY_DIR}
                       REQUIRES unity test_utils nvs_flash ulp esp_common
                      )
