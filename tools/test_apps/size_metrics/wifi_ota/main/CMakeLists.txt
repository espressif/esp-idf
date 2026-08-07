idf_component_register(SRCS "main.c"
                       PRIV_REQUIRES esp_event esp_https_ota esp_netif esp_wifi mbedtls nvs_flash)
