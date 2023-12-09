set(srcs "rtc_clk_init.c"
         "rtc_clk.c"
         "pmu_param.c"
         "pmu_init.c"
         "pmu_sleep.c"
         "rtc_time.c"
         "chip_info.c"
         )

if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs "esp_crypto_lock.c")

    if(CONFIG_ESP_SYSTEM_MEMPROT_FEATURE)
        list(APPEND srcs "esp_memprot.c" "../esp_memprot_conv.c")
    endif()

endif()

list(REMOVE_ITEM srcs
        "pmu_param.c" # TODO: IDF-7531
        "pmu_sleep.c" # TODO: IDF-7531
        "pmu_init.c"  # TODO: IDF-7531
    )

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
