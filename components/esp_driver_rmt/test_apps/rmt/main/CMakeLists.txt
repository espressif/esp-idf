set(srcs "test_app_main.c"
         "test_rmt_common.c"
         "test_rmt_tx.c"
         "test_rmt_rx.c"
         "test_util_rmt_encoders.c")

if(CONFIG_RMT_TX_ISR_CACHE_SAFE AND CONFIG_RMT_RX_ISR_CACHE_SAFE)
    list(APPEND srcs "test_rmt_cache_safe.c")
endif()

if(CONFIG_SOC_LIGHT_SLEEP_SUPPORTED AND CONFIG_PM_ENABLE)
    list(APPEND srcs "test_rmt_sleep.c")
endif()

if(CONFIG_SOC_BITSCRAMBLER_SUPPORTED AND CONFIG_SOC_RMT_SUPPORT_DMA)
    list(APPEND srcs "test_rmt_bitscrambler.c")
endif()

idf_component_register(SRCS "${srcs}"
                       PRIV_REQUIRES unity esp_driver_rmt esp_driver_gpio esp_driver_bitscrambler esp_timer esp_psram
                       WHOLE_ARCHIVE)

if(CONFIG_SOC_BITSCRAMBLER_SUPPORTED AND CONFIG_SOC_RMT_SUPPORT_DMA)
    target_bitscrambler_add_src("test_tx.bsasm")
endif()
