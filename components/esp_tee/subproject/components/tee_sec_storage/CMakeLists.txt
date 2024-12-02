idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

set(srcs)
set(priv_requires efuse mbedtls spi_flash)

if(esp_tee_build)
    list(APPEND srcs "tee_sec_storage.c")
    list(APPEND priv_requires log tee_flash_mgr)
else()
    list(APPEND srcs "tee_sec_storage_wrapper.c")
    set(priv_requires esp_tee)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS include
                       PRIV_REQUIRES ${priv_requires}
                       )
