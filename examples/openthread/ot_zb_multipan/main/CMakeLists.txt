idf_component_register(SRCS "ot_zb_multipan.c"
                       PRIV_REQUIRES openthread esp_netif nvs_flash vfs esp_timer
                       esp_driver_uart mdns esp_wifi protocol_examples_common
                       INCLUDE_DIRS ".")
