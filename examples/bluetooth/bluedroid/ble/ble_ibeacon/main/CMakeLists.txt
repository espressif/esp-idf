idf_component_register(SRCS "esp_ibeacon_api.c"
                            "ibeacon_demo.c"
                    PRIV_REQUIRES bt nvs_flash
                    INCLUDE_DIRS ".")
