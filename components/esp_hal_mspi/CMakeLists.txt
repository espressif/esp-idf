idf_build_get_property(target IDF_TARGET)
idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

set(srcs)
set(includes "include" "${target}/include")

set(priv_requires)
if(${target} STREQUAL "esp32")
    list(APPEND priv_requires esp_hal_gpio)
endif()

if(esp_tee_build)
    if(CONFIG_SECURE_TEE_EXT_FLASH_MEMPROT_SPI1)
        list(APPEND srcs "spi_flash_hal.c" "spi_flash_hal_iram.c")
    endif()
elseif(NOT BOOTLOADER_BUILD)
    if(NOT CONFIG_APP_BUILD_TYPE_PURE_RAM_APP)
        if(CONFIG_SOC_SPI_FLASH_SUPPORTED)
            list(APPEND srcs "spi_flash_hal.c" "spi_flash_hal_iram.c")
        endif()
        if(CONFIG_SOC_FLASH_ENC_SUPPORTED)
            list(APPEND srcs "spi_flash_encrypt_hal_iram.c")
        endif()
    endif()

    if(CONFIG_SOC_GPSPI_SUPPORTED AND NOT CONFIG_IDF_TARGET_ESP32)
        list(APPEND srcs "spi_flash_hal_gpspi.c")
    endif()

endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${includes}
                    REQUIRES soc hal
                    PRIV_REQUIRES "${priv_requires}")
