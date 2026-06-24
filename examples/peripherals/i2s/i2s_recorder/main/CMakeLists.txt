idf_component_register(SRCS "i2s_recorder_main.c"
                    PRIV_REQUIRES esp_driver_i2s fatfs i2s_examples_common
                    INCLUDE_DIRS)
