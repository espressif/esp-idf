idf_component_register(SRCS "ana_cmpr_etm_periodic_scan_main.c"
                       PRIV_REQUIRES esp_driver_ana_cmpr esp_driver_gptimer esp_driver_gpio
                       INCLUDE_DIRS ".")
