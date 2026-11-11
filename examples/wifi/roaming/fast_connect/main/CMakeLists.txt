idf_component_register(SRCS "fast_connect_example.c"
                    PRIV_REQUIRES esp_wifi esp_netif esp_event esp_timer nvs_flash
                    INCLUDE_DIRS ".")
