idf_component_register(
     SRCS
        sdmmc_test_cd_wp_common.c
        sdmmc_test_rw_common.c
     PRIV_REQUIRES
          sdmmc sdmmc_test_board esp_timer unity test_utils
     WHOLE_ARCHIVE TRUE
)

if(CONFIG_SOC_GPSPI_SUPPORTED)
     target_sources(
          ${COMPONENT_LIB} PRIVATE
               sdmmc_test_rw_spi.c
               sdmmc_test_begin_end_spi.c
               sdmmc_test_probe_spi.c
               sdmmc_test_cd_wp_spi.c
     )
endif()

if(CONFIG_SOC_SDMMC_HOST_SUPPORTED)
     target_sources(
          ${COMPONENT_LIB} PRIVATE
               sdmmc_test_begin_end_sd.c
               sdmmc_test_rw_sd.c
               sdmmc_test_probe_sd.c
               sdmmc_test_cd_wp_sd.c
     )
endif()
