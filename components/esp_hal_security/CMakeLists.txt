idf_build_get_property(target IDF_TARGET)
idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

set(srcs)
set(includes)

# target specific include must be added before the generic one
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()
list(APPEND includes "include")

# Security HAL source files
if(esp_tee_build)
    if(CONFIG_SOC_AES_SUPPORTED)
        list(APPEND srcs "aes_hal.c")
    endif()

    if(CONFIG_SOC_SHA_SUPPORTED)
        list(APPEND srcs "sha_hal.c")
    endif()

    if(CONFIG_SOC_HMAC_SUPPORTED)
        list(APPEND srcs "hmac_hal.c")
    endif()

    if(CONFIG_SOC_DIG_SIGN_SUPPORTED)
        list(APPEND srcs "ds_hal.c")
    endif()

    if(CONFIG_SOC_ECC_SUPPORTED)
        list(APPEND srcs "ecc_hal.c")
    endif()

    if(CONFIG_SOC_APM_SUPPORTED)
        list(APPEND srcs "apm_hal.c")
    endif()
elseif(NOT BOOTLOADER_BUILD)
    if(CONFIG_SOC_ECC_SUPPORTED)
        list(APPEND srcs "ecc_hal.c")
    endif()

    if(CONFIG_SOC_ECDSA_SUPPORTED)
        list(APPEND srcs "ecdsa_hal.c")
    endif()

    if(CONFIG_SOC_MPI_SUPPORTED)
        list(APPEND srcs "mpi_hal.c")
    endif()

    if(CONFIG_SOC_SHA_SUPPORTED)
        list(APPEND srcs "sha_hal.c")
    endif()

    if(CONFIG_SOC_AES_SUPPORTED)
        list(APPEND srcs "aes_hal.c")
    endif()

    if(CONFIG_SOC_HMAC_SUPPORTED AND NOT CONFIG_IDF_TARGET_ESP32S2)
        # For ESP32-S2 we do not have HMAC HAL layer implementation yet
        list(APPEND srcs "hmac_hal.c")
    endif()

    if(CONFIG_SOC_DIG_SIGN_SUPPORTED AND NOT CONFIG_IDF_TARGET_ESP32S2)
        # For ESP32-S2 we do not have DS HAL layer implementation yet
        list(APPEND srcs "ds_hal.c")
    endif()

    if(CONFIG_SOC_APM_SUPPORTED)
        list(APPEND srcs "apm_hal.c")
    endif()
endif()

# Key Manager and HUK HAL (available in both bootloader and app builds)
if(CONFIG_SOC_KEY_MANAGER_SUPPORTED)
    list(APPEND srcs "key_mgr_hal.c")
    list(APPEND srcs "huk_hal.c")
endif()

# MPU HAL (available in both bootloader and app builds)
if(CONFIG_SOC_MPU_SUPPORTED)
    list(APPEND srcs "mpu_hal.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       REQUIRES soc hal)
