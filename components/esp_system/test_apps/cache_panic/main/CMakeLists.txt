set(srcs "test_cache_disabled.c"
         "test_app_main.c")

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity spi_flash
                       WHOLE_ARCHIVE)
