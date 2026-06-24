idf_component_register(
    SRCS "app_main.c" "test_esp_local_ctrl.c"
    # Access the private transport struct so we can define a null transport
    # without requiring BLE or HTTPD hardware in unit tests.
    PRIV_INCLUDE_DIRS "$ENV{IDF_PATH}/components/esp_local_ctrl/src"
    PRIV_REQUIRES unity esp_local_ctrl
    WHOLE_ARCHIVE
)
