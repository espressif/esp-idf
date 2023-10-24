idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

idf_component_register(SRCS "sdmmc_cmd.c"
                            "sdmmc_common.c"
                            "sdmmc_init.c"
                            "sdmmc_io.c"
                            "sdmmc_mmc.c"
                            "sdmmc_sd.c"
                    INCLUDE_DIRS include
                    REQUIRES driver
                    PRIV_REQUIRES soc esp_timer)
