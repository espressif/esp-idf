set(srcs "test_ringbuf_main.c"
         "test_ringbuf.c")

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES esp_ringbuf esp_driver_gptimer spi_flash unity
                       WHOLE_ARCHIVE)
