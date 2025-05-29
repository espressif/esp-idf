idf_component_register(SRCS "advanced_https_ota_example.c" "ble_helper/bluedroid_gatts.c" "ble_helper/nimble_gatts.c"
                    "ble_helper/ble_api.c"
                    INCLUDE_DIRS "." "./ble_helper/include/"
                    PRIV_REQUIRES esp_http_client app_update esp_https_ota
                                  nvs_flash esp_netif esp_wifi efuse bt
                                  protocomm mbedtls
                    # Embed the server root certificate into the final binary
                    EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem)
