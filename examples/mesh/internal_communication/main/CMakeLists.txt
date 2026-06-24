idf_component_register(SRCS "mesh_light.c"
                            "mesh_main.c"
                    PRIV_REQUIRES esp_wifi esp_driver_gpio esp_driver_ledc nvs_flash
                    INCLUDE_DIRS "." "include")
