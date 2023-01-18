set(srcs "essl.c")

if(CONFIG_SOC_GPSPI_SUPPORTED)
    list(APPEND srcs "essl_spi.c")
endif()

if(CONFIG_SOC_SDIO_SLAVE_SUPPORTED)
    list(APPEND srcs "essl_sdio.c" "essl_sdio_defs.c")
endif()

idf_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "." "include/esp_serial_slave_link"
    REQUIRES sdmmc driver
)
