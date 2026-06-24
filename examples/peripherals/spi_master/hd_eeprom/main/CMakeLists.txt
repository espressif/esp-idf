set(srcs "spi_eeprom_main.c")

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES esp_driver_spi esp_driver_gpio eeprom
                       INCLUDE_DIRS ".")
