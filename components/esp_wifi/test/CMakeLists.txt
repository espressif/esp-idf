idf_component_register(SRC_DIRS .
                       INCLUDE_DIRS . ${CMAKE_CURRENT_BINARY_DIR}
                       REQUIRES unity test_utils nvs_flash ulp esp_common
                      )
