idf_component_register(SRCS "test_fatfs_flash_wl.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity spi_flash fatfs vfs test_fatfs_common
                       WHOLE_ARCHIVE)
