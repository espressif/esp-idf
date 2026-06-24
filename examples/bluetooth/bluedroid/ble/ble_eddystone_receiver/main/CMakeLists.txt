idf_component_register(SRCS "esp_eddystone_api.c"
                            "esp_eddystone_demo.c"
                        PRIV_REQUIRES bt nvs_flash
                        INCLUDE_DIRS "")
