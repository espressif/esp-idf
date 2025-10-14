idf_component_register(SRCS "cmd_ota.c" "app_main.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES app_update console esp_driver_uart esp_event esp_http_client
                                     esp_https_ota esp_netif esp_wifi mbedtls nvs_flash)
