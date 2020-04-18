idf_component_register(SRCS "esp_netif_test.c"
                       INCLUDE_DIRS "."
                       PRIV_INCLUDE_DIRS  "../../private_include" "."
                       PRIV_REQUIRES unity esp_netif nvs_flash)
