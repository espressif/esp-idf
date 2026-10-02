idf_component_register(SRCS "test_fatfs_bdl.c" "test_fatfs_instance.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity fatfs vfs esp_blockdev esp_blockdev_util esp_partition
                       WHOLE_ARCHIVE)
