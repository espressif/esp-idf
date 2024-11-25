idf_component_register(SRCS "mesh_light.c"
                            "mesh_main.c"
                    PRIV_REQUIRES esp_wifi esp_driver_gpio nvs_flash esp_driver_ledc
                    INCLUDE_DIRS "." "include")
