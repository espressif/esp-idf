idf_component_register(SRCS "src/diskio.c"
                            "src/diskio_rawflash.c"
                            "src/diskio_sdmmc.c"
                            "src/diskio_wl.c"
                            "src/ff.c"
                            "src/ffsystem.c"
                            "src/ffunicode.c"
                            "src/vfs_fat.c"
                            "src/vfs_fat_sdmmc.c"
                            "src/vfs_fat_spiflash.c"
                    INCLUDE_DIRS src
                    REQUIRES wear_levelling sdmmc)
