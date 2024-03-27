set(srcs "test_app_main.c"
         "test_jpeg_decode.c"
    )

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES esp_driver_jpeg unity esp_psram test_utils
                       WHOLE_ARCHIVE)
