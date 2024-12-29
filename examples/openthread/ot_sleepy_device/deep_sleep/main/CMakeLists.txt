idf_component_register(SRCS "esp_ot_sleepy_device.c"
                       PRIV_REQUIRES esp_event esp_timer openthread nvs_flash ulp
                       INCLUDE_DIRS ".")
