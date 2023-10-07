idf_component_register(SRCS "logic_analyzer_example_main.c"
                    PRIV_REQUIRES esp_probe driver nvs_flash esp_netif protocol_examples_common
                    INCLUDE_DIRS ".")
