set(srcs "test_app_main.c"
         "test_ledc.c")

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(
    SRCS ${srcs}
    PRIV_REQUIRES unity esp_driver_pcnt esp_driver_ledc esp_driver_gpio esp_timer esp_psram
    WHOLE_ARCHIVE
)
