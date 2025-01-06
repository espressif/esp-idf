idf_component_register(SRCS "i2s_es7210_record_example.c"
                       PRIV_REQUIRES esp_driver_i2s esp_driver_gpio fatfs i2s_examples_common
                       INCLUDE_DIRS
)
