idf_component_register(SRCS "uart_async_rxtxtasks_main.c"
                    REQUIRES esp_driver_uart esp_driver_gpio
                    INCLUDE_DIRS ".")
