idf_component_register(SRCS "ethernet_example_main.c"
                       PRIV_REQUIRES esp_wifi esp_eth nvs_flash
                       INCLUDE_DIRS ".")
