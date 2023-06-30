set(srcs test_mqtt5_client_broker.c test_mqtt5.c)

idf_component_register(SRCS "${srcs}"
                    PRIV_REQUIRES cmock test_utils mqtt nvs_flash app_update esp_eth esp_netif spi_flash common)
