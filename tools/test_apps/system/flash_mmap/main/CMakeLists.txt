idf_component_register(SRCS "test_flash_mmap.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity spi_flash esp_partition)
