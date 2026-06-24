idf_build_get_property(target IDF_TARGET)

set(srcs "test_app_main.c")

if(CONFIG_SOC_LIGHT_SLEEP_SUPPORTED)
    list(APPEND srcs "test_esp_timer_light_sleep.c")
endif()

if(CONFIG_SOC_SYSTIMER_SUPPORT_ETM)
    list(APPEND srcs "test_esp_timer_etm.c")
endif()

if(CONFIG_SOC_GPTIMER_SUPPORTED OR CONFIG_IDF_TARGET_LINUX)
    list(APPEND srcs "test_esp_timer.c")
endif()

if(NOT ${target} STREQUAL "linux")

    if(CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD)
        list(APPEND srcs "test_esp_timer_dfs.c")
    endif()

    set(priv_requires cmock test_utils esp_timer spi_flash esp_psram esp_driver_gpio esp_pm)
    list(APPEND srcs "test_ets_timer.c")
else()
    set(priv_requires cmock test_utils esp_timer)
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_INCLUDE_DIRS "../../private_include" "../include"
                       PRIV_REQUIRES "${priv_requires}"
                       WHOLE_ARCHIVE)
