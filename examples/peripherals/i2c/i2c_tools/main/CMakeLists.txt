idf_component_register(SRCS "i2ctools_example_main.c"
                            "cmd_i2ctools.c"
                       PRIV_REQUIRES esp_driver_i2c console
                       INCLUDE_DIRS ".")
