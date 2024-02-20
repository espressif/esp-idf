set(srcs "test_app_main.c"
         "test_sdio_sdhost.c")

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity test_driver_utils driver esp_serial_slave_link sdmmc test_utils esp_timer
                       WHOLE_ARCHIVE)
