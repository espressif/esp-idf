idf_component_register(SRCS "rs485_example.c"
                    REQUIRES nvs_flash esp_driver_uart
                    INCLUDE_DIRS ".")
