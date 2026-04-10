set(EFUSE_SOC_SRCS
    "esp_efuse_utility.c"
    "esp_efuse_fields.c"
)

if(NOT CONFIG_ESP32P4_SELECTS_REV_LESS_V3)
    list(APPEND EFUSE_SOC_SRCS
        "esp_efuse_table_v3.0.c"
        "esp_efuse_rtc_calib.c"
    )
else()
    list(APPEND EFUSE_SOC_SRCS
        "esp_efuse_table.c"
        "esp_efuse_rtc_calib.c"
    )
endif()
