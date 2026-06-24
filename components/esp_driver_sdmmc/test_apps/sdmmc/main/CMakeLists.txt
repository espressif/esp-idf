set(srcs "test_app_main.c" "test_sd_driver_resource.c")

set(priv_requires
        # tests reside in this component, also available for `sdmmc_console`
        sdmmc_tests
        # general
        unity
        # for PSRAM tests
        esp_psram
        esp_driver_sdmmc
)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES ${priv_requires}
                       WHOLE_ARCHIVE TRUE)
