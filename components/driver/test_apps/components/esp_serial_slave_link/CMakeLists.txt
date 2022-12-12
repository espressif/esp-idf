idf_component_register(
    SRCS "essl.c" "essl_sdio.c" "essl_spi.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "." "include/esp_serial_slave_link"
    REQUIRES sdmmc driver
)
