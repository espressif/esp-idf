idf_component_register(SRCS "async_color_convert_example_main.c"
                       PRIV_REQUIRES esp_driver_dma mbedtls
                       INCLUDE_DIRS "."
                       EMBED_FILES "assets/sample_96x64_uyvy.yuv")
