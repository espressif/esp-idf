idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

set(srcs "esp_attestation.c"
         "esp_att_utils_part_info.c"
         "esp_att_utils_crypto.c"
         "esp_att_utils_json.c")

set(include_dirs "include")
set(priv_include_dirs "private_include")
set(priv_requires bootloader_support efuse esp_app_format esp_bootloader_format json_generator log mbedtls spi_flash)

if(esp_tee_build)
    list(APPEND priv_requires tee_sec_storage tee_flash_mgr)
else()
    list(APPEND priv_requires app_update esp_partition)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include_dirs}
                       PRIV_INCLUDE_DIRS ${priv_include_dirs}
                       PRIV_REQUIRES ${priv_requires})
