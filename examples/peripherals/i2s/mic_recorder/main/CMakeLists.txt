idf_component_register(SRCS "mic_recorder_example_main.c"
                       PRIV_REQUIRES esp_driver_i2s esp_driver_i2c esp_driver_gpio mbedtls
                       INCLUDE_DIRS .)
