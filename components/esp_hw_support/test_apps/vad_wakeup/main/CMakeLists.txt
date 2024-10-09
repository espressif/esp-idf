set(srcs "test_app_main.c"
         "test_vad_wakeup.c")

idf_component_register(SRCS ${srcs}
                       REQUIRES unity esp_driver_i2s esp_driver_uart ulp esp_timer
                       WHOLE_ARCHIVE
                       EMBED_FILES "test_vad_8k.pcm")
