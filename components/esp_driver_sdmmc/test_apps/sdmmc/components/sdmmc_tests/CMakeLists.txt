set(srcs)


if(CONFIG_SOC_SDMMC_HOST_SUPPORTED)
    list(APPEND srcs "sdmmc_test_begin_end_sd.c"
                     "sdmmc_test_cd_wp_sd.c"
                     "sdmmc_test_probe_sd.c"
                     "sdmmc_test_rw_sd.c"
                     "sdmmc_test_erase_sd.c"
                     "sdmmc_test_trim_sd.c"
                     "sdmmc_test_discard_sd.c"
                     "sdmmc_test_sanitize_sd.c"
                     "sdmmc_test_various_cmds.c")
endif()

set(priv_requires "sdmmc"
                  "esp_driver_sdmmc"
                  "sdmmc_test_boards"
                  "common_test_flows"
                  "unity"
)

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES ${priv_requires}
                       WHOLE_ARCHIVE TRUE)
