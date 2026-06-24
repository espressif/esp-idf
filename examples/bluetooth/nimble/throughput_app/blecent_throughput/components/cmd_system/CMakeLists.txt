idf_component_register(SRCS "cmd_system.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_driver_uart esp_driver_gpio
                    REQUIRES console spi_flash)
