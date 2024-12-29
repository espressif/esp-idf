set(srcs "test_app_main.c"
         "test_sdio_slave.c")

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES test_driver_utils driver esp_hw_support
                       WHOLE_ARCHIVE)
