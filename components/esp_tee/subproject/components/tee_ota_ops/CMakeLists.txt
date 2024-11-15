idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

set(srcs)
set(priv_requires)
set(include_dirs  "include")

if(esp_tee_build)
    list(APPEND srcs "esp_tee_ota_ops.c")
    list(APPEND priv_requires bootloader_support esp_tee log spi_flash tee_flash_mgr)
else()
    list(APPEND srcs "esp_tee_ota_ops_wrapper.c")
    list(APPEND priv_requires esp_tee)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include_dirs}
                       PRIV_REQUIRES ${priv_requires})
