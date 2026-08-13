idf_build_get_property(target IDF_TARGET)
idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

if(${target} STREQUAL "linux")
    return()
endif()

set(srcs
    "src/esp_image_format.c"
    "src/bootloader_sha.c"
    "src/bootloader_sha_flash.c"
)

# Secure Boot V1 + V2 sources. Bootloader build picks up the on-boot verifier; app
# build picks up the on-update verifier. ESP-TEE app builds compile V2 (RSA / ECDSA-V2)
# but skip V1 entirely (V1 hardware is ESP32 / ESP32-S2).
if(BOOTLOADER_BUILD)
    if(CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME)
        list(APPEND srcs "src/secure_boot_v1/secure_boot_signatures_bootloader.c")
    endif()
    if(CONFIG_SECURE_BOOT_V1_ENABLED)
        list(APPEND srcs "src/secure_boot_v1/secure_boot.c")
    endif()
    if(CONFIG_SECURE_BOOT_V2_ENABLED)
        list(APPEND srcs "src/secure_boot_v2/secure_boot_signatures_bootloader.c"
                         "src/secure_boot_v2/secure_boot.c")
    endif()
elseif(esp_tee_build)
    if(CONFIG_SECURE_BOOT_V2_ENABLED)
        if(CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME OR CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME)
            list(APPEND srcs "src/secure_boot_v2/secure_boot_signatures_bootloader.c"
                             "src/secure_boot_v2/secure_boot.c")
        endif()
    endif()
else()
    if(CONFIG_SECURE_SIGNED_ON_UPDATE)
        if(CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME)
            list(APPEND srcs "src/secure_boot_v1/secure_boot_signatures_app.c")
        endif()
        if(CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME)
            list(APPEND srcs "src/secure_boot_v2/secure_boot_signatures_app.c"
                             "src/secure_boot_v2/secure_boot_rsa_signature.c")
        endif()
        if(CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME)
            list(APPEND srcs "src/secure_boot_v2/secure_boot_signatures_app.c"
                             "src/secure_boot_v2/secure_boot_ecdsa_signature.c")
        endif()
    endif()
    # secure_boot.c — app-side on-update signature-block sanity check
    list(APPEND srcs "src/secure_boot.c")
endif()

# Public REQUIRES bootloader_support — esp_image_format.h includes esp_flash_partitions.h
# from bootloader_support's public include path.
set(requires bootloader_support esp_app_format esp_bootloader_format)

set(priv_requires spi_flash efuse esp_hal_security)

if(BOOTLOADER_BUILD)
    list(APPEND priv_requires micro-ecc)
elseif(NOT esp_tee_build)
    # heap is needed by the SHA primitive (PSA hash op allocations).
    list(APPEND priv_requires heap mbedtls app_update)
endif()

idf_component_register(
    SRCS "${srcs}"
    PRIV_INCLUDE_DIRS "private_include"
    REQUIRES "${requires}"
    PRIV_REQUIRES "${priv_requires}"
)

if(NOT BOOTLOADER_BUILD AND NOT esp_tee_build)
    if(CONFIG_SECURE_SIGNED_ON_UPDATE)
        if(CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME OR CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME OR
           CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME)
            target_link_libraries(${COMPONENT_LIB} PRIVATE idf::app_update)
        endif()
    endif()
endif()
