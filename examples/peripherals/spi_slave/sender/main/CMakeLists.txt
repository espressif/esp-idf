idf_component_register(SRCS "app_main.c"
                    PRIV_REQUIRES esp_driver_spi esp_driver_gpio esp_timer
                    INCLUDE_DIRS ".")
