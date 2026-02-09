set(srcs "test_app_main.c"
         "test_ledc.cpp"
         "test_ledc_utils.cpp")

if(CONFIG_SOC_LIGHT_SLEEP_SUPPORTED)
    list(APPEND srcs "test_ledc_sleep.cpp")
endif()

if(CONFIG_SOC_ETM_SUPPORTED AND CONFIG_SOC_LEDC_SUPPORT_ETM)
    list(APPEND srcs "test_ledc_etm.cpp")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(
    SRCS ${srcs}
    PRIV_REQUIRES unity esp_driver_ledc esp_driver_gpio esp_timer esp_psram esp_driver_uart
    WHOLE_ARCHIVE
)
