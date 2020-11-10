set(srcs "spi_eeprom_main.c")

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ".")
