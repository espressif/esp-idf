idf_component_register(SRCS "test_app_main.c" "test_touch_v2.c" "touch_scope.c"
                       PRIV_REQUIRES unity driver esp_driver_gpio esp_driver_uart
                       WHOLE_ARCHIVE
)
