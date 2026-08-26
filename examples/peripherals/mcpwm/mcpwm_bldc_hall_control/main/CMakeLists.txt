idf_component_register(SRCS "mcpwm_bldc_hall_control_example_main.c"
                       PRIV_REQUIRES esp_driver_mcpwm esp_timer esp_driver_gpio
                       INCLUDE_DIRS ".")
