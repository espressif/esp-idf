idf_component_register(SRCS "smtp_client_example_main.c"
                       PRIV_REQUIRES esp_event esp_netif nvs_flash mbedtls
                       EMBED_TXTFILES server_root_cert.pem esp_logo.png)
