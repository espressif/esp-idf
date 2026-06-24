# Embed CA, certificate & key directly into binary
idf_component_register(SRCS "roaming_example.c"
                    PRIV_REQUIRES esp_wifi wpa_supplicant nvs_flash
                    INCLUDE_DIRS ".")
