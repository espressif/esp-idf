idf_component_register(SRCS "i2s_std_example_main.c"
                    PRIV_REQUIRES esp_driver_i2s esp_driver_gpio
                    INCLUDE_DIRS "." "$ENV{IDF_PATH}/examples/peripherals/i2s/common")
