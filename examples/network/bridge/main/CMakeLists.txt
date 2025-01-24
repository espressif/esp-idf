idf_component_register(SRCS "bridge_example_main.c"
                            "bridge_console_cmd.c"
                       PRIV_REQUIRES esp_eth console esp_netif esp_wifi nvs_flash
                       INCLUDE_DIRS ".")
