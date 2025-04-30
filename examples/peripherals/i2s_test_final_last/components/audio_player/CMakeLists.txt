idf_component_register(
    SRCS "audio_player.c"
    INCLUDE_DIRS "include"
    REQUIRES driver esp_timer freertos esp_common
)
