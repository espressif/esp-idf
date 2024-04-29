set(srcs
    "test_app_main.c"
    "test_ets_timer.c"
)

if(CONFIG_SOC_LIGHT_SLEEP_SUPPORTED)
    list(APPEND srcs "test_esp_timer_light_sleep.c")
endif()

if(CONFIG_SOC_SYSTIMER_SUPPORT_ETM)
    list(APPEND srcs "test_esp_timer_etm.c")
endif()

if(CONFIG_SOC_GPTIMER_SUPPORTED)
    list(APPEND srcs "test_esp_timer.c")
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_INCLUDE_DIRS "../../private_include"
                       PRIV_REQUIRES cmock test_utils esp_timer spi_flash esp_psram esp_driver_gpio
                       WHOLE_ARCHIVE)
