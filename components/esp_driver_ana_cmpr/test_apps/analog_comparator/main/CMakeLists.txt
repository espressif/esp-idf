set(srcs "test_app_main.c"
         "test_ana_cmpr_common.c"
         "test_ana_cmpr.c")

if(CONFIG_ANA_CMPR_ISR_IRAM_SAFE)
    list(APPEND srcs "test_ana_cmpr_iram.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity esp_driver_gpio esp_driver_ana_cmpr
                       WHOLE_ARCHIVE)
