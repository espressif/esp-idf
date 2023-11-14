idf_component_register(SRCS cmd_sdmmc.c
                       INCLUDE_DIRS .
                       PRIV_REQUIRES console sdmmc esp_driver_sdmmc esp_driver_gpio sdmmc_test_board
                       driver # driver will be replaced with esp_driver_sdspi later
)
