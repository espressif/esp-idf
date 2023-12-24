idf_build_get_property(target IDF_TARGET)

set(srcs "diskio/diskio.c"
        "diskio/diskio_rawflash.c"
        "diskio/diskio_wl.c"
        "src/ff.c"
        "src/ffunicode.c")

set(include_dirs "diskio" "src")

set(requires "wear_levelling")

# for linux, we do not have support for vfs and sdmmc, for real targets, add respective sources
if(${target} STREQUAL "linux")
    list(APPEND srcs "port/linux/ffsystem.c")
else()
    list(APPEND srcs "port/freertos/ffsystem.c"
            "diskio/diskio_sdmmc.c"
            "vfs/vfs_fat.c"
            "vfs/vfs_fat_sdmmc.c"
            "vfs/vfs_fat_spiflash.c")

    list(APPEND include_dirs "vfs")

    list(APPEND requires "sdmmc" "esp_driver_sdmmc" "esp_driver_sdspi")

    list(APPEND priv_requires "vfs" "esp_driver_gpio")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include_dirs}
                       REQUIRES ${requires}
                       PRIV_REQUIRES ${priv_requires}
                      )
