idf_component_register(SRCS "dpp_enrollee_main.c"
                    PRIV_REQUIRES esp_wifi wpa_supplicant nvs_flash
                    INCLUDE_DIRS ".")
