idf_component_register(SRCS "gptimer_capture_hc_sr04.c"
                       PRIV_REQUIRES esp_driver_gptimer esp_driver_gpio
                       INCLUDE_DIRS ".")
