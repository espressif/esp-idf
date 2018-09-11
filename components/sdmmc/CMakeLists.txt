set(COMPONENT_SRCS "sdmmc_cmd.c"
                   "sdmmc_common.c"
                   "sdmmc_init.c"
                   "sdmmc_io.c"
                   "sdmmc_mmc.c"
                   "sdmmc_sd.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_REQUIRES driver)
register_component()
