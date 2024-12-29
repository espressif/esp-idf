idf_component_register(SRCS "jpeg_decode_main.c"
                    PRIV_REQUIRES fatfs esp_driver_jpeg esp_psram
                    INCLUDE_DIRS ".")
