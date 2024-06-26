idf_component_register(SRCS "bootloader_hooks_example_main.c"
                    INCLUDE_DIRS ".")
nvs_create_partition_image(nvs ../nvs_data.csv FLASH_IN_PROJECT)
