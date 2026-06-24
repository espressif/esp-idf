idf_component_register(SRCS "esp_ot_sleepy_device.c"
                       PRIV_REQUIRES esp_event openthread nvs_flash
                       INCLUDE_DIRS ".")

if(CONFIG_ESP_SLEEP_DEBUG)
    idf_component_optional_requires(PRIVATE esp_timer)
endif()
