idf_component_register(SRCS "test_sdmmc_app.c"
                       PRIV_INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_blockdev unity sdmmc esp_driver_sdmmc sdmmc_tests
                       WHOLE_ARCHIVE
                      )
