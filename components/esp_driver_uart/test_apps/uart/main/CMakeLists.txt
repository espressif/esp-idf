# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(
    SRCS "test_app_main.c" "test_uart.c"
    REQUIRES esp_driver_uart unity esp_psram
    WHOLE_ARCHIVE
)
