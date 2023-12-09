idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ".")

set(nvs_partition_name nvs_ro)
set(nvs_data_csv ../nvs_data.csv)
nvs_create_partition_image(${nvs_partition_name} ${nvs_data_csv} FLASH_IN_PROJECT)

set(image ../filesystem_image)

set(fatfs_wl_partition_name fatfs_ro)
set(fatfs_raw_partition_name fatfs_raw_ro)
fatfs_create_spiflash_image(${fatfs_wl_partition_name} ${image} FLASH_IN_PROJECT)
fatfs_create_rawflash_image(${fatfs_raw_partition_name} ${image} FLASH_IN_PROJECT)

set(spiffs_partition_name spiffs_ro)
spiffs_create_partition_image(${spiffs_partition_name} ${image} FLASH_IN_PROJECT)
