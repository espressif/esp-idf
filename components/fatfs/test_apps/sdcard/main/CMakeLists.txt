idf_component_register(SRCS "test_fatfs_sdcard_main.c" "test_fatfs_sdspi.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity fatfs vfs sdmmc driver test_fatfs_common
                       WHOLE_ARCHIVE)

if(CONFIG_SOC_SDMMC_HOST_SUPPORTED)
    target_sources(${COMPONENT_LIB} PRIVATE "test_fatfs_sdmmc.c")
endif()
