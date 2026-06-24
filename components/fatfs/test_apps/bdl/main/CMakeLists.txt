idf_component_register(SRCS "test_fatfs_bdl.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity fatfs vfs esp_blockdev esp_partition
                       WHOLE_ARCHIVE)
