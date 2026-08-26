idf_component_register(SRCS "app_main.c"
                    PRIV_REQUIRES esp_driver_gpio esp_driver_sdmmc esp_driver_uart
                    INCLUDE_DIRS ".")
