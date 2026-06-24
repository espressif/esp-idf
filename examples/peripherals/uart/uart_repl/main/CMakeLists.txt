idf_component_register(SRCS "uart_repl_example_main.c"
                    REQUIRES esp_driver_uart esp_driver_gpio console
                    INCLUDE_DIRS ".")
