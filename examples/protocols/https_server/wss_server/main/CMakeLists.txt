idf_component_register(SRCS "wss_server_example.c" "keep_alive.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_https_server nvs_flash esp_timer esp_netif esp_eth esp_wifi
                    EMBED_TXTFILES "certs/servercert.pem"
                                   "certs/prvtkey.pem")
