set(srcs "test_app_main.c"
         "test_temperature_sensor.c"
         "test_temperature_phy.c")

if(CONFIG_SOC_TEMPERATURE_SENSOR_SUPPORT_ETM)
    list(APPEND srcs "test_temperature_etm.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity esp_wifi test_utils nvs_flash esp_driver_tsens esp_driver_gpio
                       WHOLE_ARCHIVE)
