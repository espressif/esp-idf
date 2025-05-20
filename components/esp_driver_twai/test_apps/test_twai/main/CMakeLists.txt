set(srcs "test_app_main.c")

if(CONFIG_SOC_TWAI_SUPPORTED)
    list(APPEND srcs "test_twai_common.c")
endif()

if(CONFIG_SOC_TWAI_SUPPORT_FD)
    list(APPEND srcs "test_twai_fd.c")
endif()

idf_component_register(
    SRCS ${srcs}
    PRIV_REQUIRES esp_driver_twai esp_timer esp_driver_uart esp_psram
    WHOLE_ARCHIVE
)
