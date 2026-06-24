idf_component_register(
    SRCS app_main.c svpwm/esp_svpwm.c foc/esp_foc.c
    INCLUDE_DIRS "svpwm" "foc"
    PRIV_REQUIRES esp_driver_mcpwm esp_driver_gpio
)
