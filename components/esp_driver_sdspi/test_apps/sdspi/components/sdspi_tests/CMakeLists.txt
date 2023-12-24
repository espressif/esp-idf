set(srcs)

if(CONFIG_SOC_GPSPI_SUPPORTED)
    list(APPEND srcs "sdmmc_test_begin_end_spi.c"
                     "sdmmc_test_cd_wp_spi.c"
                     "sdmmc_test_probe_spi.c"
                     "sdmmc_test_rw_spi.c"
                     "sdmmc_test_erase_spi.c"
                     "sdmmc_test_erase_common_spi.c")
endif()

set(priv_requires "sdmmc"
                  "esp_driver_sdspi"
                  "sdmmc_test_boards"
                  "common_test_flows"
                  "unity"
)

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES ${priv_requires}
                       WHOLE_ARCHIVE TRUE)
