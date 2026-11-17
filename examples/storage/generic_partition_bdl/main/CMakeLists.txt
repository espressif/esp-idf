set(srcs "generic_partition_bdl_main.c")
if(CONFIG_EXAMPLE_STORAGE_MEDIA_SDCARD)
    list(APPEND srcs "sd_card_bdl.c")
endif()

# fatfs already provides the SDMMC/SDSPI driver dependencies on hardware targets.
idf_component_register(SRCS ${srcs}
                    PRIV_REQUIRES vfs fatfs esp_partition esp_blockdev esp_blockdev_util
                    INCLUDE_DIRS ".")
