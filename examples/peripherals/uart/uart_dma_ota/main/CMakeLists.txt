idf_component_register(SRCS "uart_dma_ota_example_main.c"
                    REQUIRES esp_driver_uart app_update
                    INCLUDE_DIRS ".")
