# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(
    SRCS "test_app_main.c" "test_rs485.c"
    REQUIRES esp_driver_uart unity test_utils
    WHOLE_ARCHIVE
)
