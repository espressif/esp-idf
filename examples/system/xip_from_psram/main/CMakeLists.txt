idf_component_register(SRCS "xip_from_psram_example_main.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_psram esp_partition spi_flash esp_timer)
