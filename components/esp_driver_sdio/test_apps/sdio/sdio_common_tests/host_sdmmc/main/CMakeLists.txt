set(srcs "test_app_main.c"
         "test_sdio_sdhost.c")

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity esp_serial_slave_link test_utils esp_timer esp_driver_sdmmc
                       WHOLE_ARCHIVE)
