idf_component_register(SRCS "netif_init_c99.c" "main.cpp" "netif_init_cpp.cpp"
                    PRIV_REQUIRES nvs_flash esp_wifi wpa_supplicant
                    INCLUDE_DIRS ".")
