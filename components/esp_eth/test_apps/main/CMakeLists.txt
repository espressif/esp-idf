idf_component_register(SRCS "esp_eth_test_main.c"
                            "esp_eth_test_apps.c"
                            "esp_eth_test_l2.c"
                            "esp_eth_test_utils.c"
                            "esp_eth_test_esp_emac.c"
                            "esp_eth_test_emac_sleep_retention.c"
                            "esp_eth_test_emac_init.cpp"
                       INCLUDE_DIRS "."
                       PRIV_INCLUDE_DIRS "."
                       PRIV_REQUIRES unity esp_eth esp_netif esp_http_client esp_driver_gpio esp_driver_uart
                       EMBED_TXTFILES dl_espressif_com_root_cert.pem
                       WHOLE_ARCHIVE)
