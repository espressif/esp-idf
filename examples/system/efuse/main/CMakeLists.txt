idf_component_register(SRCS "efuse_main.c"
                            "esp_efuse_custom_table.c"
                       PRIV_REQUIRES efuse
                       INCLUDE_DIRS "."
                                    "include")

if(CONFIG_SECURE_BOOT OR CONFIG_SECURE_FLASH_ENC_ENABLED OR CONFIG_IDF_TARGET_ESP32C2)
    idf_component_optional_requires(PRIVATE bootloader_support)
endif()
