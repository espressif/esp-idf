idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

set(srcs)
set(priv_requires esp_tee)

if(esp_tee_build)
    list(APPEND srcs "tee_sec_storage.c")
    list(APPEND priv_requires efuse esp_partition log mbedtls nvs_flash spi_flash tee_flash_mgr)
else()
    list(APPEND srcs "tee_sec_storage_wrapper.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS include
                       PRIV_REQUIRES ${priv_requires}
                       )
