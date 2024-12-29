idf_component_register(SRCS "main.c"
                    PRIV_REQUIRES esp-tls esp_wifi nvs_flash esp_eth esp_http_server
                    INCLUDE_DIRS ".")
