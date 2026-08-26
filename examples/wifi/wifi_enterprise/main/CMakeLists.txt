# Embed CA, certificate & key directly into binary
idf_component_register(SRCS "wifi_enterprise_main.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_wifi wpa_supplicant nvs_flash
                    EMBED_TXTFILES ca.pem client.crt client.key)
