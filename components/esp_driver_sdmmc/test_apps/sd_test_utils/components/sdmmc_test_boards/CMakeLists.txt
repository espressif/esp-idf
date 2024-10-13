idf_component_register(SRCS sdmmc_test_board.c sdmmc_test_board_defs.c
                       INCLUDE_DIRS include
                       REQUIRES esp_driver_sdmmc esp_driver_sdspi esp_driver_gpio unity)
