idf_component_register(SRCS "spi_eeprom.c"
                    LDFRAGMENTS "linker.lf"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_driver_gpio
                    REQUIRES esp_driver_spi)
