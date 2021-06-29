idf_component_register(SRCS "chip_drivers.c" "spi_flash_chip_eon.c"
                    PRIV_REQUIRES spi_flash
                    LDFRAGMENTS linker.lf
                    INCLUDE_DIRS "")

idf_component_add_link_dependency(FROM spi_flash)
