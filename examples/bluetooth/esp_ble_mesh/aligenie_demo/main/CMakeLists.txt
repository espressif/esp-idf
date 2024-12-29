idf_component_register(SRCS "board.c"
                            "aligenie_demo.c"
                    PRIV_REQUIRES vendor_model esp_driver_gpio mbedtls
                    INCLUDE_DIRS "." "include")
