set(srcs "test_app_main.c" "test_nvs_bootloader.c")
set(embed_txtfiles "")

if(CONFIG_NVS_ENCRYPTION OR SOC_HMAC_SUPPORTED)
    list(APPEND srcs "test_encrypted_nvs_bootloader.c")
    list(APPEND embed_txtfiles "nvs_partition.bin" "partition_encrypted.bin" "partition_encrypted_hmac.bin")
endif()

if(CONFIG_NVS_SEC_KEY_PROTECT_USING_FLASH_ENC)
    list(APPEND embed_txtfiles "encryption_keys.bin")
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "."
                       REQUIRES unity nvs_flash nvs_sec_provider efuse
                       EMBED_TXTFILES "${embed_txtfiles}"
                       WHOLE_ARCHIVE
                       )

nvs_create_partition_image(nvs ../partition_nvs_data.csv FLASH_IN_PROJECT)
