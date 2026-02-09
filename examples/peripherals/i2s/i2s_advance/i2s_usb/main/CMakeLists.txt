idf_component_register(SRCS "i2s_usb_example_main.c"
                    PRIV_REQUIRES esp_driver_i2s esp_driver_i2c esp_driver_gpio
                    INCLUDE_DIRS ".")
