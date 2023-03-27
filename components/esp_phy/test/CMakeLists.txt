idf_component_register(SRC_DIRS .
                       PRIV_INCLUDE_DIRS . ${CMAKE_CURRENT_BINARY_DIR}
                       PRIV_REQUIRES cmock test_utils nvs_flash ulp esp_common esp_phy esp_wifi
                      )
