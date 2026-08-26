idf_component_register(SRCS "test_fatfs_flash_wl_readonly.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity spi_flash fatfs vfs
                       WHOLE_ARCHIVE)
