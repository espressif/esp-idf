idf_component_register(SRCS
        "test_std_filesystem_main.cpp"
        "test_ops.cpp"
        "test_paths.cpp"
        "test_status.cpp"
    INCLUDE_DIRS "."
    PRIV_REQUIRES vfs fatfs
    WHOLE_ARCHIVE)

fatfs_create_spiflash_image(storage ${CMAKE_CURRENT_LIST_DIR}/test_fs_image FLASH_IN_PROJECT)
