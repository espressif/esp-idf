set(EFUSE_SOC_SRCS
    "esp_efuse_utility.c"
    "esp_efuse_fields.c"
    "esp_efuse_rtc_calib.c"
)

if(CONFIG_ESP32P4_REV_MIN_FULL GREATER_EQUAL 300)
    list(APPEND EFUSE_SOC_SRCS
        "esp_efuse_table_v3.0.c"
    )
else()
    list(APPEND EFUSE_SOC_SRCS
        "esp_efuse_table.c"
    )
endif()
