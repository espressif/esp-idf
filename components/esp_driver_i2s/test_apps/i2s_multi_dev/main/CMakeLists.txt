idf_component_register(SRCS "test_app_main.c" "test_i2s_multi_dev.c"
                       PRIV_REQUIRES unity test_utils esp_driver_gpio esp_driver_i2s
                       INCLUDE_DIRS "."
                       WHOLE_ARCHIVE
)
