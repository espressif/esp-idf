set(srcs "")
set(priv_requires "soc")

if(NOT BOOTLOADER_BUILD)
    if(CONFIG_IDF_TARGET_ESP32H2 OR CONFIG_IDF_TARGET_ESP32P4 OR CONFIG_IDF_TARGET_ESP32C5)
        list(APPEND srcs "src/crypto/${IDF_TARGET}/clk.c")
    endif()

    if(CONFIG_SOC_CRYPTO_DPA_PROTECTION_SUPPORTED)
        list(APPEND srcs "src/crypto/esp_dpa_protection.c")
    endif()

    list(APPEND srcs "src/crypto/esp_crypto_lock.c")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES ${priv_requires})

if(NOT BOOTLOADER_BUILD)
    if(CONFIG_SOC_CRYPTO_DPA_PROTECTION_SUPPORTED)
        target_link_libraries(${COMPONENT_LIB} PRIVATE "-u esp_crypto_dpa_prot_include_impl")
    endif()
endif()
