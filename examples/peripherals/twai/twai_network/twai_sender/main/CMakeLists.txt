idf_component_register(
    SRCS "twai_sender.c"
    REQUIRES esp_driver_twai esp_timer
    INCLUDE_DIRS "."
)
