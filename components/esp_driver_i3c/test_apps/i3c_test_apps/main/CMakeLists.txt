set(srcs "test_app_main.c"
         "test_i3c_master_common.cpp"
         "test_i3c_multi.cpp"
    )

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity test_utils esp_driver_i3c esp_driver_uart esp_driver_gpio
                       WHOLE_ARCHIVE)
