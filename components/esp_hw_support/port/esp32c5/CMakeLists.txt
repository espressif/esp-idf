if(CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION)
    set(srcs "rtc_clk_init.c"
        "rtc_clk.c"
        "rtc_time.c"
        "pmu_init.c"
        "pmu_sleep.c"
        "pmu_param.c"
        "chip_info.c"
    )
endif()
if(CONFIG_IDF_TARGET_ESP32C5_MP_VERSION)
    set(srcs "rtc_clk_init.c"
        "rtc_time.c"
        "rtc_clk.c"
        "chip_info.c"
    )
endif()


if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs "sar_periph_ctrl.c"
                     "esp_crypto_lock.c")
endif()

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC . include private_include)
