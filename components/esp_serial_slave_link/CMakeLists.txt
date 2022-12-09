idf_component_register(SRCS "essl.c"
                "essl_sdio.c"
            INCLUDE_DIRS "include"
            REQUIRES "sdmmc"
                "driver"
            PRIV_INCLUDE_DIRS "."
                "include/esp_serial_slave_link"
)

