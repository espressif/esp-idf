set(srcs "tusb_msc_main.c")
set(requires fatfs console)

if(CONFIG_EXAMPLE_STORAGE_MEDIA_SPIFLASH)
    list(APPEND srcs "tusb_msc_storage_spiffs.c")
    list(APPEND requires "wear_levelling")
endif()

if(CONFIG_EXAMPLE_STORAGE_MEDIA_SDMMCCARD)
    list(APPEND srcs "tusb_msc_storage_sdmmc.c")
endif()

idf_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS .
    REQUIRES "${requires}"
)
