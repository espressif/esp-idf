idf_component_get_property(heap_dir heap COMPONENT_DIR)

set(priv_requires esp_hw_support esp_hal_security esp_hal_dma)

set(include_dirs "${COMPONENT_DIR}/port/include"
                 "${COMPONENT_DIR}/mbedtls/include"
                 "${COMPONENT_DIR}/mbedtls/library")

# Crypto port headers
set(crypto_port_inc_dirs "${COMPONENT_DIR}/port/aes/include"
                         "${COMPONENT_DIR}/port/aes/dma/include"
                         "${COMPONENT_DIR}/port/sha/core/include")

# Supporting headers
list(APPEND crypto_port_inc_dirs "${heap_dir}/include")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "${include_dirs}" "${crypto_port_inc_dirs}"
                       PRIV_REQUIRES "${priv_requires}")

# Only build mbedtls libraries
set(ENABLE_TESTING OFF CACHE BOOL "mbedtls: enable testing")
set(ENABLE_PROGRAMS OFF CACHE BOOL "mbedtls: enable programs")

# Use pre-generated source files in mbedtls repository
set(GEN_FILES OFF CACHE BOOL "mbedtls: use pre-generated source files")

# Needed to for include_next includes to work from within mbedtls
include_directories("${COMPONENT_DIR}/port/include")

# Import mbedtls library targets
add_subdirectory(mbedtls)

# Set TF_PSA_CRYPTO_CONFIG_FILE before processing subdirectories to prevent override
set(
    TF_PSA_CRYPTO_USER_CONFIG_FILE "${COMPONENT_DIR}/esp_tee/esp_tee_mbedtls_config.h"
    CACHE STRING "Path to the PSA Crypto configuration file"
    FORCE
)

set(mbedtls_targets mbedtls tfpsacrypto builtin mbedx509 everest p256m)

target_sources(tfpsacrypto PRIVATE "${COMPONENT_DIR}/port/esp_hardware.c")

foreach(target ${mbedtls_targets})
    target_compile_definitions(${target} PUBLIC
                                    -DMBEDTLS_CONFIG_FILE="${COMPONENT_DIR}/esp_tee/esp_tee_mbedtls_config.h")
    set_config_files_compile_definitions(${target})
    target_compile_definitions(${target} PUBLIC MBEDTLS_MAJOR_VERSION=4)
    if(CONFIG_COMPILER_STATIC_ANALYZER AND CMAKE_C_COMPILER_ID STREQUAL "GNU") # TODO IDF-10087
        target_compile_options(${target} PRIVATE "-fno-analyzer")
    endif()
    if(CONFIG_MBEDTLS_COMPILER_OPTIMIZATION_SIZE)
        message(STATUS "Setting -Os for ${target}")
        target_compile_options(${target} PRIVATE "-Os")
    elseif(CONFIG_MBEDTLS_COMPILER_OPTIMIZATION_PERF)
        target_compile_options(${target} PRIVATE "-O2")
    endif()
endforeach()

target_link_libraries(${COMPONENT_LIB} INTERFACE ${mbedtls_targets})

target_link_libraries(tfpsacrypto PRIVATE idf::esp_security)

target_include_directories(tfpsacrypto PRIVATE ${crypto_port_inc_dirs})

# Shared GDMA layer for TEE
target_sources(tfpsacrypto PRIVATE "${COMPONENT_DIR}/esp_tee/esp_tee_crypto_shared_gdma.c")
target_include_directories(tfpsacrypto PUBLIC "${COMPONENT_DIR}/port/psa_driver/include")

# AES implementation
if(CONFIG_SOC_AES_SUPPORTED)
    target_sources(tfpsacrypto PRIVATE "${COMPONENT_DIR}/port/aes/esp_aes.c"
                                        "${COMPONENT_DIR}/port/aes/dma/esp_aes_dma_core.c"
                                        "${COMPONENT_DIR}/port/aes/esp_aes_gcm.c"
                                        "${COMPONENT_DIR}/port/aes/esp_aes_common.c"
                                        "${COMPONENT_DIR}/port/aes/esp_aes_xts.c")
    target_include_directories(tfpsacrypto PRIVATE "${COMPONENT_DIR}/port/include/aes")
    if(CONFIG_MBEDTLS_HARDWARE_AES)
        target_compile_definitions(tfpsacrypto PRIVATE ESP_AES_DRIVER_ENABLED)
        target_sources(tfpsacrypto PRIVATE
                "${COMPONENT_DIR}/port/psa_driver/esp_aes/psa_crypto_driver_esp_aes.c"
                "${COMPONENT_DIR}/port/psa_driver/esp_aes/psa_crypto_driver_esp_aes_gcm.c"
        )
        if(CONFIG_MBEDTLS_HARDWARE_SHA)
            target_sources(tfpsacrypto PRIVATE
                "${COMPONENT_DIR}/port/psa_driver/esp_aes/psa_crypto_driver_esp_cmac.c"
            )
        endif()
    endif()

endif()
# SHA implementation
if(CONFIG_SOC_SHA_SUPPORTED)
    target_compile_definitions(tfpsacrypto PRIVATE ESP_SHA_DRIVER_ENABLED)
    target_sources(tfpsacrypto PRIVATE
        "${COMPONENT_DIR}/port/psa_driver/esp_sha/psa_crypto_driver_esp_sha.c"
        "${COMPONENT_DIR}/port/psa_driver/esp_sha/core/psa_crypto_driver_esp_sha1.c"
        "${COMPONENT_DIR}/port/psa_driver/esp_sha/core/psa_crypto_driver_esp_sha256.c"
        "${COMPONENT_DIR}/port/psa_driver/esp_sha/core/psa_crypto_driver_esp_sha512.c"
        "${COMPONENT_DIR}/port/sha/core/sha.c"
        "${COMPONENT_DIR}/port/sha/esp_sha.c")
endif()

if(CONFIG_MBEDTLS_ROM_MD5)
    target_compile_definitions(tfpsacrypto PRIVATE ESP_MD5_DRIVER_ENABLED)
    target_sources(tfpsacrypto PRIVATE
        "${COMPONENT_DIR}/port/psa_driver/esp_md/psa_crypto_driver_esp_md5.c"
    )
endif()

if(CONFIG_SOC_ECC_SUPPORTED)
    target_sources(tfpsacrypto PRIVATE "${COMPONENT_DIR}/port/ecc/esp_ecc.c"
                                    "${COMPONENT_DIR}/port/ecc/ecc_alt.c")
endif()

if(CONFIG_SOC_HMAC_SUPPORTED)
    # HMAC-based PBKDF2 implementation
    target_sources(tfpsacrypto PRIVATE "${COMPONENT_DIR}/port/esp_hmac_pbkdf2.c")
endif()

# PSA Attestation
target_include_directories(tfpsacrypto PUBLIC "${COMPONENT_DIR}/port/psa_attestation")
