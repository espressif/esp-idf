set(srcs "test_app_main.c"
         "test_ana_cmpr_utils.c"
         "test_ana_cmpr.c")

if(CONFIG_ANA_CMPR_ISR_CACHE_SAFE)
    list(APPEND srcs "test_ana_cmpr_iram.c")
endif()

if(CONFIG_SOC_ANA_CMPR_SUPPORT_ETM AND CONFIG_SOC_TIMER_SUPPORT_ETM)
    # Analog Comparator event test relies on GPTIMER task
    list(APPEND srcs "test_ana_cmpr_etm.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity esp_driver_gpio esp_driver_ana_cmpr esp_driver_gptimer esp_pm esp_psram
                       WHOLE_ARCHIVE)
