set(srcs "rtc_clk_init.c"
         "rtc_clk.c"
        #  "rtc_init.c" // TODO: IDF-5645
        #  "rtc_pm.c" // TODO: IDF-5645
        #  "rtc_sleep.c" // TODO: IDF-5645
         "rtc_time.c"
         "chip_info.c")

if(NOT BOOTLOADER_BUILD)
    # list(APPEND srcs "esp_hmac.c" // TODO: IDF-5355
    #                  "esp_crypto_lock.c"
    #                  "esp_ds.c") // TODO: IDF-5360
    list(APPEND srcs "sar_periph_ctrl.c")

    if(CONFIG_ESP_SYSTEM_MEMPROT_FEATURE)
        list(APPEND srcs "esp_memprot.c" "../esp_memprot_conv.c")
    endif()

endif()

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC . private_include)
target_include_directories(${COMPONENT_LIB} PRIVATE ../hal)
