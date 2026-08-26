idf_component_register(SRCS "main.c" "pan_netif.c"
                    PRIV_REQUIRES bt bt_app_core_utils nvs_flash esp_netif esp_event esp_wifi lwip
                    INCLUDE_DIRS ".")
