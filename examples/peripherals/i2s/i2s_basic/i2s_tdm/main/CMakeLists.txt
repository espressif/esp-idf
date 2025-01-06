idf_component_register(SRCS "i2s_tdm_example_main.c"
                    PRIV_REQUIRES esp_driver_i2s esp_driver_gpio i2s_examples_common
                    INCLUDE_DIRS ".")
