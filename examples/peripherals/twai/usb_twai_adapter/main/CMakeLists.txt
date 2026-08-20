idf_component_register(
    SRCS "candlelight_main.c"
         "candlelight_twai.c"
         "gs_usb.c"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_twai esp_timer
)
