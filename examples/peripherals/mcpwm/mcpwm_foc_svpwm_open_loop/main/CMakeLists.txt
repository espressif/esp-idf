idf_component_register(
    SRCS app_main.c svpwm/esp_svpwm.c foc/esp_foc.c
    INCLUDE_DIRS "svpwm" "foc"
    REQUIRES driver
)
