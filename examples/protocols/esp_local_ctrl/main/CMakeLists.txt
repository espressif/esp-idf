idf_component_register(SRCS "app_main.c" "esp_local_ctrl_service.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "certs/cacert.pem" "certs/prvtkey.pem")
