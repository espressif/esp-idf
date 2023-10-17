set(srcs "test_app_main.c"
         "test_flash_encryption.c")

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity spi_flash bootloader_support esp_partition
                       WHOLE_ARCHIVE)
