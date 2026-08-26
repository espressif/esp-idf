idf_component_register(SRCS "app_hf_msg_set.c"
                            "bt_app_core.c"
                            "bt_app_hf.c"
                            "gpio_pcm_config.c"
                            "main.c"
                    PRIV_REQUIRES bt nvs_flash esp_driver_gpio console esp_ringbuf
                    INCLUDE_DIRS ".")
