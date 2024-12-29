idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_https_server esp_wifi nvs_flash esp_eth
                    EMBED_TXTFILES "certs/servercert.pem"
                                   "certs/prvtkey.pem")
