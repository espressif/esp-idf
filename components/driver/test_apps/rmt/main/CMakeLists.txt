set(srcs "test_app_main.c"
         "test_rmt_common.c"
         "test_rmt_tx.c"
         "test_rmt_rx.c"
         "test_util_rmt_encoders.c")

if(CONFIG_RMT_ISR_IRAM_SAFE)
    list(APPEND srcs "test_rmt_iram.c")
endif()

idf_component_register(SRCS "${srcs}"
                       PRIV_REQUIRES unity driver esp_timer esp_psram
                       WHOLE_ARCHIVE)
