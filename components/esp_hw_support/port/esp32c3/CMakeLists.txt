set(srcs "rtc_clk_init.c"
         "rtc_clk.c"
         "rtc_init.c"
         "rtc_sleep.c"
         "rtc_time.c"
         "chip_info.c"
         )

if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs "esp_crypto_lock.c"
                     "sar_periph_ctrl.c")

    # init constructor for wifi
    list(APPEND srcs "adc2_init_cal.c")

    if(CONFIG_ESP_SYSTEM_MEMPROT_FEATURE)
        list(APPEND srcs "esp_memprot.c" "../esp_memprot_conv.c")
    endif()

endif()

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC . include)
