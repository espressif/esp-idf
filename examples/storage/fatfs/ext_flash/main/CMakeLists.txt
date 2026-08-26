idf_component_register(SRCS "fatfs_ext_flash_example_main.c"
                       PRIV_REQUIRES vfs nvs_flash spi_flash fatfs)
