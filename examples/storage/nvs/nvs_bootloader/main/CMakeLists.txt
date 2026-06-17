idf_component_register(SRCS "bootloader_hooks_example_main.c"
                    INCLUDE_DIRS ".")

if(NOT CONFIG_NVS_ENCRYPTION)
    nvs_create_partition_image(nvs ../nvs_data.csv FLASH_IN_PROJECT)
else()
    if(CONFIG_NVS_SEC_KEY_PROTECT_USING_FLASH_ENC)
        esp_partition_register_target("nvs_key" ${PROJECT_DIR}/main/encryption_keys.bin FLASH_IN_PROJECT)
        esp_partition_register_target("nvs" ${PROJECT_DIR}/main/nvs_encrypted.bin FLASH_IN_PROJECT)
    else()  # NVS Encryption using HMAC (CONFIG_NVS_SEC_KEY_PROTECT_USING_HMAC)
        esp_partition_register_target("nvs" ${PROJECT_DIR}/main/nvs_encrypted_hmac.bin FLASH_IN_PROJECT)
    endif()
endif()
