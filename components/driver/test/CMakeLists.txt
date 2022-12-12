idf_component_register(
    SRC_DIRS .
    PRIV_INCLUDE_DIRS include
    PRIV_REQUIRES test_utils driver nvs_flash esp_timer esp_event test_driver_utils esp_serial_slave_link
)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
