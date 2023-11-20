idf_component_register(SRCS "bt_app_av.c"
                            "bt_app_core.c"
                            "main.c"
                    PRIV_REQUIRES esp_driver_i2s bt nvs_flash esp_ringbuf
                                  driver  # TODO: IDF-8370 remove driver when esp_driver_dac created
                    INCLUDE_DIRS ".")
