set(srcs "test_app_main.c"
         "test_ledc.c"
         "test_ledc_utils.c")

if(CONFIG_SOC_LIGHT_SLEEP_SUPPORTED)
    list(APPEND srcs "test_ledc_sleep.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(
    SRCS ${srcs}
    PRIV_REQUIRES unity esp_driver_pcnt esp_driver_ledc esp_driver_gpio esp_timer esp_psram
    WHOLE_ARCHIVE
)
