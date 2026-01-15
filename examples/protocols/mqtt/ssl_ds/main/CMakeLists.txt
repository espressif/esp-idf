idf_component_register(SRCS "app_main.c"
                    PRIV_REQUIRES mqtt esp_netif nvs_flash
                    INCLUDE_DIRS ".")
