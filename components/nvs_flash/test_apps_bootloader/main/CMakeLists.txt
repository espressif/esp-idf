idf_component_register(SRCS "test_app_main.c" "test_nvs_bootloader.c"
                       INCLUDE_DIRS "."
                       REQUIRES unity nvs_flash
                       WHOLE_ARCHIVE
                       )

nvs_create_partition_image(nvs ../partition_nvs_data.csv FLASH_IN_PROJECT)
