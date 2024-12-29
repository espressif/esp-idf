idf_component_register(SRCS "flash_encrypt_main.c"
                            "flash_encrypt_fatfs.c"
                       PRIV_REQUIRES vfs nvs_flash spi_flash efuse bootloader_support
                                     fatfs espcoredump nvs_sec_provider
                       INCLUDE_DIRS ".")
