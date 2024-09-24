idf_component_register(SRCS "main.c"
                            "bt_app_core.c"
                    PRIV_REQUIRES bt nvs_flash esp_ringbuf vfs
                    INCLUDE_DIRS ".")
