idf_component_register(SRCS "bootloader_hooks_example_main.c"
                    INCLUDE_DIRS ".")

if(NOT CONFIG_NVS_ENCRYPTION)
    nvs_create_partition_image(nvs ../nvs_data.csv FLASH_IN_PROJECT)
else()
    if(CONFIG_NVS_SEC_KEY_PROTECT_USING_FLASH_ENC)
        esptool_py_flash_to_partition(flash "nvs_key" ${PROJECT_DIR}/main/encryption_keys.bin)
        esptool_py_flash_to_partition(flash "nvs" ${PROJECT_DIR}/main/nvs_encrypted.bin)
    else()  # NVS Encryption using HMAC (CONFIG_NVS_SEC_KEY_PROTECT_USING_HMAC)
        esptool_py_flash_to_partition(flash "nvs" ${PROJECT_DIR}/main/nvs_encrypted_hmac.bin)
    endif()
endif()
