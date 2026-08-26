idf_component_register(SRCS "wifi_nvs_config_main.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_wifi nvs_flash)
