set(srcs "rtc_clk_init.c"
         "rtc_clk.c"
         "pmu_param.c"
         "pmu_init.c"
         "pmu_sleep.c"
         "rtc_time.c"
         "chip_info.c"
         "ocode_init.c"
         )

if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs "sar_periph_ctrl.c"
                     "esp_crypto_lock.c")

    if(CONFIG_ESP_SYSTEM_MEMPROT_FEATURE)
        list(APPEND srcs "esp_memprot.c" "../esp_memprot_conv.c")
    endif()

endif()

# TODO: [ESP32C61] IDF-9250
if(CONFIG_IDF_TARGET_ESP32C61)
    list(REMOVE_ITEM srcs
        "pmu_sleep.c"
        "sar_periph_ctrl.c"
    )
endif()

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC . include private_include)
