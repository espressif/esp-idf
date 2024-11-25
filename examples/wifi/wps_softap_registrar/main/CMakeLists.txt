idf_component_register(SRCS "wps.c"
                    PRIV_REQUIRES esp_wifi wpa_supplicant nvs_flash
                    INCLUDE_DIRS ".")
