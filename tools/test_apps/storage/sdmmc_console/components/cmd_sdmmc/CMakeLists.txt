idf_component_register(SRCS cmd_sdmmc.c
                       INCLUDE_DIRS .
                       PRIV_REQUIRES console sdmmc esp_driver_sdmmc esp_driver_gpio esp_driver_sdspi
                                     sdmmc_test_boards

)
