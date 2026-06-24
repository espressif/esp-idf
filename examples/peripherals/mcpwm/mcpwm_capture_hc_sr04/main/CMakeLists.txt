idf_component_register(SRCS "mcpwm_capture_hc_sr04.c"
                       PRIV_REQUIRES esp_driver_mcpwm esp_driver_gpio
                       INCLUDE_DIRS ".")
