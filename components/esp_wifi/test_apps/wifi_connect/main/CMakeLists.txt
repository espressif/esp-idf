idf_component_register(SRCS
                        "app_main.c"
                        "test_wifi_conn.c"
                        "test_bss_max_idle.c"

                         PRIV_INCLUDE_DIRS . ${CMAKE_CURRENT_BINARY_DIR}
                         PRIV_REQUIRES cmock test_utils nvs_flash ulp esp_common esp_wifi esp_event
                         WHOLE_ARCHIVE)

add_definitions(-DCONFIG_ESP_WIFI_BSS_MAX_IDLE_SUPPORT)
