idf_component_register(SRCS "touch_sens_sleep.c"
                    PRIV_REQUIRES esp_driver_touch_sens esp_driver_uart
                    INCLUDE_DIRS ".")
