idf_component_register(SRCS "logic_analyzer_example_main.c"
                    PRIV_REQUIRES esp_probe nvs_flash esp_netif protocol_examples_common
                                   esp_driver_parlio esp_driver_gpio
                    INCLUDE_DIRS ".")
