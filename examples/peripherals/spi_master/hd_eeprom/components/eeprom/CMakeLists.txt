idf_component_register(SRCS "spi_eeprom.c"
                    LDFRAGMENTS "linker.lf"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES driver)
