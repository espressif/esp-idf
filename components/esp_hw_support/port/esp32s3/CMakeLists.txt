target_include_directories(${COMPONENT_LIB} PUBLIC .)
target_include_directories(${COMPONENT_LIB} PUBLIC private_include)

set(srcs
    "rtc_clk.c"
    "rtc_clk_init.c"
    "rtc_init.c"
    "rtc_pm.c"
    "rtc_sleep.c"
    "rtc_time.c"
    "rtc_wdt.c"
    "chip_info.c"
   )

if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs "../async_memcpy_impl_gdma.c"
        "dport_access.c"
        "esp_hmac.c"
        "esp_ds.c"
        "esp_crypto_lock.c"
        "spiram.c")

    if(CONFIG_SPIRAM_MODE_QUAD)
        list(APPEND srcs "spiram_psram.c")
    elseif(CONFIG_SPIRAM_MODE_OCT)
        list(APPEND srcs "opiram_psram.c")
    endif()

    if(CONFIG_ESP_SYSTEM_MEMPROT_FEATURE)
        list(APPEND srcs "esp_memprot.c" "../esp_memprot_conv.c")
    endif()

endif()

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")
target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
