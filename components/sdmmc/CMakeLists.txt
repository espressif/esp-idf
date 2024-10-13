idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs "sdmmc_cmd.c"
         "sdmmc_common.c"
         "sdmmc_init.c"
         "sdmmc_io.c"
         "sdmmc_mmc.c"
         "sdmmc_sd.c"
         "sd_pwr_ctrl/sd_pwr_ctrl.c")

if(CONFIG_SOC_GP_LDO_SUPPORTED)
    list(APPEND srcs "sd_pwr_ctrl/sd_pwr_ctrl_by_on_chip_ldo.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS include
                       PRIV_REQUIRES soc esp_timer esp_mm)
