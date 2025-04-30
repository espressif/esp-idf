idf_component_register(
    SRCS "audio_player.c"
    INCLUDE_DIRS "include"
    REQUIRES driver freertos esp_timer fatfs sdmmc vfs wear_levelling spi_flash
)
