# Embed the server root certificate into the final binary
idf_build_get_property(project_dir PROJECT_DIR)
idf_component_register(SRCS "simple_mqtt_ota_example.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES app_update mqtt esp_netif mbedtls nvs_flash
                                    esp_wifi)
