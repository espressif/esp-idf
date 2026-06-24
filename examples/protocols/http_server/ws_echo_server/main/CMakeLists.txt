idf_component_register(SRCS "ws_echo_server.c"
                    PRIV_REQUIRES esp_http_server esp_wifi nvs_flash esp_eth
                    INCLUDE_DIRS ".")
