idf_component_register(SRCS "nan_pmk_test.c"
                            "test_main.c"
                       INCLUDE_DIRS "."
                       REQUIRES unity esp_wifi nvs_flash esp_netif esp_event
                       PRIV_REQUIRES wpa_supplicant
                       PRIV_INCLUDE_DIRS ../../../../wpa_supplicant/src/)
