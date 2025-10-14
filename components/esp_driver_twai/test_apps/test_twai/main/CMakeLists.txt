set(srcs "test_app_main.c")

if(CONFIG_SOC_TWAI_SUPPORTED)
    list(APPEND srcs "test_twai_common.cpp" "test_twai_network.cpp")
endif()

if(CONFIG_SOC_TWAI_SUPPORT_FD)
    list(APPEND srcs "test_twai_fd.cpp")
endif()

idf_component_register(
    SRCS ${srcs}
    PRIV_REQUIRES esp_driver_twai esp_timer esp_driver_uart esp_psram esp_driver_gpio
    WHOLE_ARCHIVE
)
