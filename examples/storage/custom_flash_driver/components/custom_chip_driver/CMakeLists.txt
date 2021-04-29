idf_component_register(SRCS "chip_drivers.c" "spi_flash_chip_eon.c"
                    PRIV_REQUIRES spi_flash
                    LDFRAGMENTS linker.lf
                    INCLUDE_DIRS "")

idf_component_get_property(spi_flash_lib spi_flash COMPONENT_LIB)
set_property(TARGET ${spi_flash_lib} APPEND PROPERTY INTERFACE_LINK_LIBRARIES $<LINK_ONLY:${COMPONENT_LIB}>)
