idf_component_register(SRCS "app_main.c" "esp_local_ctrl_service.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_local_ctrl nvs_flash
                    EMBED_TXTFILES "certs/servercert.pem" "certs/prvtkey.pem")
