idf_component_register(SRCS "isp_dma_main.c"
                       PRIV_REQUIRES esp_driver_isp esp_mm esp_psram mbedtls
                       INCLUDE_DIRS ".")
