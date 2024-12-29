idf_component_register(SRCS "jpeg_encode_main.c"
                    PRIV_REQUIRES fatfs esp_driver_jpeg
                    INCLUDE_DIRS ".")
