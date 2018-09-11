set(COMPONENT_SRCS "src/diskio.c"
                   "src/diskio_rawflash.c"
                   "src/diskio_sdmmc.c"
                   "src/diskio_wl.c"
                   "src/ff.c"
                   "src/ffsystem.c"
                   "src/ffunicode.c"
                   "src/vfs_fat.c"
                   "src/vfs_fat_sdmmc.c"
                   "src/vfs_fat_spiflash.c")
set(COMPONENT_ADD_INCLUDEDIRS src)

set(COMPONENT_REQUIRES wear_levelling sdmmc)

register_component()
