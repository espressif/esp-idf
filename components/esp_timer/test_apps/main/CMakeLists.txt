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

if(CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD)
    list(APPEND srcs "test_esp_timer_dfs.c")
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_INCLUDE_DIRS "../../private_include" "../include"
                       PRIV_REQUIRES cmock test_utils esp_timer spi_flash esp_psram esp_driver_gpio esp_pm
                       WHOLE_ARCHIVE)

if(CONFIG_IDF_TARGET_LINUX AND NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    # Linux host heap integrity checks are stubs, so use ASan to catch dump overflows.
    set(asan_options -fsanitize=address -fno-omit-frame-pointer)
    idf_component_get_property(esp_timer_lib esp_timer COMPONENT_LIB)
    target_compile_options(${esp_timer_lib} PRIVATE ${asan_options})
    target_compile_options(${COMPONENT_LIB} PRIVATE ${asan_options})
    idf_build_set_property(LINK_OPTIONS -fsanitize=address APPEND)
endif()
