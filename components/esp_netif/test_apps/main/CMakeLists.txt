idf_component_register(SRCS "esp_netif_test.c"
                       REQUIRES test_utils
                       INCLUDE_DIRS "."
                       PRIV_INCLUDE_DIRS "$ENV{IDF_PATH}/components/esp_netif/private_include" "."
                       PRIV_REQUIRES unity esp_netif nvs_flash esp_wifi)
