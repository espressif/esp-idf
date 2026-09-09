idf_component_register(SRCS "wav_player_example_main.c"
                       PRIV_REQUIRES esp_driver_i2s esp_driver_i2c esp_driver_gpio
                       INCLUDE_DIRS .)

# Embed the WAV as-is. The firmware skips the RIFF header and plays the signed
# 16-bit PCM payload from flash; no build-time conversion and no RAM copy of the clip.
target_add_binary_data(${COMPONENT_LIB} "${CMAKE_CURRENT_LIST_DIR}/../assets/music.wav"
                       BINARY RENAME_TO "music_wav")
