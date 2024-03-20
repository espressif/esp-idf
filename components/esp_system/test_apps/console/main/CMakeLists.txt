idf_component_register(SRCS "test_app_main.c"
                       REQUIRES vfs esp_driver_uart
                       WHOLE_ARCHIVE)
