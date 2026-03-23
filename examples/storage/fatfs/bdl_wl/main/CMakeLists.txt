idf_component_register(SRCS "fatfs_bdl_wl_main.c"
                    PRIV_REQUIRES vfs fatfs esp_blockdev esp_partition wear_levelling
                    INCLUDE_DIRS ".")
