idf_component_register(SRCS "esp_eth_test_main.c"
                            "esp_eth_test_esp_emac.c"
                            "esp_eth_test_emac_sleep_retention.c"
                            "esp_eth_test_emac_init.cpp"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity esp_eth esp_netif esp_event hal esp_hw_support
                                     esp_driver_gpio esp_driver_uart esp_pm
                       WHOLE_ARCHIVE)
