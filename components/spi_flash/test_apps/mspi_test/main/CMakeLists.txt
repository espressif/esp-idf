set(srcs "test_out_of_bounds_write.c"
         "test_read_write.c"
         "test_large_flash_writes.c"
         "test_app_main.c")

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity spi_flash driver esp_partition bootloader_support app_update
                       WHOLE_ARCHIVE)
