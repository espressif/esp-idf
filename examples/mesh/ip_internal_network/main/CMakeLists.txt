idf_component_register(SRCS "mesh_main.c"
                            "mesh_netif.c"
                            "mqtt_app.c"
                    PRIV_REQUIRES esp_wifi esp-tls nvs_flash mqtt esp_driver_gpio
                    INCLUDE_DIRS "." "include")
