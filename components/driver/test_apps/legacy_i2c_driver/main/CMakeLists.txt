set(srcs "test_app_main.c"
         "test_i2c.c"
    )

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity test_utils driver esp_driver_gpio esp_hal_uart
                       WHOLE_ARCHIVE)
