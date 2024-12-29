idf_component_register(SRCS "ble_spp_server_demo.c"
                    PRIV_REQUIRES bt nvs_flash esp_driver_uart
                    INCLUDE_DIRS ".")
