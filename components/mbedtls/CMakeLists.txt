idf_build_get_property(idf_target IDF_TARGET)
idf_build_get_property(python PYTHON)
idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

if(esp_tee_build)
    include(${COMPONENT_DIR}/esp_tee/esp_tee_mbedtls.cmake)
    return()
elseif(BOOTLOADER_BUILD)   # TODO: IDF-11673
    if(CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL_BOOTLOADER)
        set(include_dirs "${COMPONENT_DIR}/mbedtls/include"
                         "port/mbedtls_rom")
        set(srcs "port/mbedtls_rom/mbedtls_rom_osi_bootloader.c")
    endif()

    idf_component_register(SRCS "${srcs}"
                           INCLUDE_DIRS "${include_dirs}"
                           PRIV_REQUIRES esp_hal_dma)
    return()
endif()

if(NOT ${IDF_TARGET} STREQUAL "linux")
    set(priv_requires soc esp_hw_support)
    if(NOT BOOTLOADER_BUILD)
        list(APPEND priv_requires esp_pm esp_driver_dma)
        set(requires esp_security)
    endif()
endif()

set(mbedtls_srcs "")
set(mbedtls_include_dirs
    "port/include"
    "mbedtls/include"
    "mbedtls/library"
    "mbedtls/tf-psa-crypto/core"
    "mbedtls/tf-psa-crypto/drivers/builtin/src/"
    )

if(CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL)
    list(APPEND mbedtls_include_dirs "port/mbedtls_rom")
endif()

if(CONFIG_MBEDTLS_CERTIFICATE_BUNDLE)
    list(APPEND mbedtls_srcs "esp_crt_bundle/esp_crt_bundle.c")
    list(APPEND mbedtls_include_dirs "esp_crt_bundle/include")
endif()

list(APPEND mbedtls_include_dirs "${COMPONENT_DIR}/port/psa_driver/include")

idf_component_register(SRCS "${mbedtls_srcs}"
    INCLUDE_DIRS "${mbedtls_include_dirs}"
    PRIV_REQUIRES "${priv_requires}"
    REQUIRES "${requires}"
    )

# Add MBEDTLS_MAJOR_VERSION definition to the component library
target_compile_definitions(${COMPONENT_LIB} INTERFACE MBEDTLS_MAJOR_VERSION=4)

# Determine the type of mbedtls component library
if(mbedtls_srcs STREQUAL "")
    # For no sources in component library we must use "INTERFACE"
    set(linkage_type INTERFACE)
else()
    set(linkage_type PUBLIC)
endif()


if(CONFIG_MBEDTLS_CERTIFICATE_BUNDLE)
    set(bundle_name "x509_crt_bundle")
    set(DEFAULT_CRT_DIR ${COMPONENT_DIR}/esp_crt_bundle)

    # Generate custom certificate bundle using the generate_cert_bundle utility
    set(GENERATE_CERT_BUNDLEPY ${python} ${COMPONENT_DIR}/esp_crt_bundle/gen_crt_bundle.py)

    if(CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEFAULT_FULL)
        list(APPEND crt_paths ${DEFAULT_CRT_DIR}/cacrt_all.pem)
    elseif(CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEFAULT_CMN)
        list(APPEND crt_paths ${DEFAULT_CRT_DIR}/cacrt_all.pem)
        list(APPEND args --filter ${DEFAULT_CRT_DIR}/cmn_crt_authorities.csv)
    endif()

    # Currently cacrt_local.pem contains deprecated certificates that are still required for certain certificate chains.
    # These chains may include cross-signed certificates, but the final certificate in the chain is deprecated.
    # When cross-signed verification is enabled (CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_CROSS_SIGNED_VERIFY),
    # the cross-signed certificate should be sufficient for verification, and the deprecated root is not needed.
    # Therefore, cacrt_local.pem is only appended if cross-signed verification is not enabled.
    if((CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEFAULT_FULL OR CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEFAULT_CMN)
        AND NOT CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_CROSS_SIGNED_VERIFY)
        list(APPEND crt_paths ${DEFAULT_CRT_DIR}/cacrt_local.pem)
    endif()

    # Add deprecated root certs if enabled. This config is not visible if the default cert
    # bundle is not selected
    if(CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEPRECATED_LIST)
        list(APPEND crt_paths ${DEFAULT_CRT_DIR}/cacrt_deprecated.pem)
    endif()

    if(CONFIG_MBEDTLS_CUSTOM_CERTIFICATE_BUNDLE)
        get_filename_component(custom_bundle_path
        ${CONFIG_MBEDTLS_CUSTOM_CERTIFICATE_BUNDLE_PATH} ABSOLUTE BASE_DIR "${project_dir}")
        list(APPEND crt_paths ${custom_bundle_path})

    endif()
    list(APPEND args --input ${crt_paths} -q --max-certs "${CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_MAX_CERTS}")

    get_filename_component(crt_bundle
        ${bundle_name}
        ABSOLUTE BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")

    # Generate bundle according to config
    add_custom_command(OUTPUT ${crt_bundle}
        COMMAND ${GENERATE_CERT_BUNDLEPY} ${args}
        DEPENDS ${custom_bundle_path}
        VERBATIM)

    add_custom_target(custom_bundle DEPENDS ${cert_bundle})
    add_dependencies(${COMPONENT_LIB} custom_bundle)


    target_add_binary_data(${COMPONENT_LIB} ${crt_bundle} BINARY)
    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES
        "${crt_bundle}")
endif()

# Only build mbedtls libraries
set(ENABLE_TESTING OFF CACHE BOOL "mbedtls: enable testing")
set(ENABLE_PROGRAMS OFF CACHE BOOL "mbedtls: enable programs")

# Use pre-generated source files in mbedtls repository
set(GEN_FILES OFF CACHE BOOL "mbedtls: use pre-generated source files")

# Make sure mbedtls finds the same Python interpreter as IDF uses
idf_build_get_property(python PYTHON)
set(Python3_EXECUTABLE ${python})

# Needed to for include_next includes to work from within mbedtls
set(include_dirs "${COMPONENT_DIR}/port/include")

if(CONFIG_MBEDTLS_CERTIFICATE_BUNDLE)
    list(APPEND include_dirs "${COMPONENT_DIR}/esp_crt_bundle/include")
endif()

list(APPEND include_dirs "${COMPONENT_DIR}/port/psa_driver/include")

include_directories(${include_dirs})

# Needed to for mbedtls_rom includes to work from within mbedtls
if(CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL)
    include_directories("${COMPONENT_DIR}/port/mbedtls_rom")
endif()

# Set TF_PSA_CRYPTO_CONFIG_FILE before processing subdirectories to prevent override
set(
    TF_PSA_CRYPTO_USER_CONFIG_FILE "mbedtls/esp_config.h"
    CACHE STRING "Path to the PSA Crypto configuration file"
    FORCE
)

# Import mbedtls library targets
add_subdirectory(mbedtls)

# Use port specific implementation of net_socket.c instead of one from mbedtls
get_target_property(src_tls mbedtls SOURCES)
list(REMOVE_ITEM src_tls net_sockets.c)
set_property(TARGET mbedtls PROPERTY SOURCES ${src_tls})

if(CONFIG_MBEDTLS_SSL_PROTO_GMTSSL1_1)
    get_target_property(src_tls mbedtls SOURCES)
    list(REMOVE_ITEM src_tls ssl_ciphersuites.c ssl_cli.c ssl_tls.c)
    set_property(TARGET mbedtls PROPERTY SOURCES ${src_tls})

    message(STATUS "Setting up mbedtls")

    # list(REMOVE_ITEM src_crypto sha512.c)
    # list(REMOVE_ITEM src_crypto cipher_wrap.c ecdsa.c ecp.c ecp_curves.c oid.c pk_wrap.c)
    # set_property(TARGET tfpsacrypto PROPERTY SOURCES ${src_crypto})

    get_target_property(src_builtin builtin SOURCES)
    message(STATUS "src_builtin: ${src_builtin}")

    get_target_property(src_x509 mbedx509 SOURCES)
    list(REMOVE_ITEM src_x509 x509_crt.c)
    set_property(TARGET mbedx509 PROPERTY SOURCES ${src_x509})
endif()

# Core libraries from the mbedTLS project
set(mbedtls_targets mbedtls mbedx509 tfpsacrypto builtin)

add_library(mbed-builtin ALIAS builtin)
set_target_properties(builtin PROPERTIES OUTPUT_NAME "mbed-builtin")

target_include_directories(tfpsacrypto PUBLIC "port/include")

message(STATUS "Setting up mbedtls configuration")
foreach(target ${mbedtls_targets})
    target_compile_definitions(${target} PUBLIC -DMBEDTLS_CONFIG_FILE="mbedtls/esp_config.h")
    set_config_files_compile_definitions(${target})
    target_compile_definitions(${target} PUBLIC MBEDTLS_MAJOR_VERSION=4)
    if(CONFIG_COMPILER_STATIC_ANALYZER AND CMAKE_C_COMPILER_ID STREQUAL "GNU") # TODO IDF-10087
        target_compile_options(${target} PRIVATE "-fno-analyzer")
    endif()
    if(CONFIG_MBEDTLS_COMPILER_OPTIMIZATION_SIZE)
        target_compile_options(${target} PRIVATE "-Os")
    elseif(CONFIG_MBEDTLS_COMPILER_OPTIMIZATION_PERF)
        target_compile_options(${target} PRIVATE "-O2")
    endif()
endforeach()

# 3rd party libraries from the mbedTLS project
list(APPEND mbedtls_targets everest p256m)

set(mbedtls_target_sources "${COMPONENT_DIR}/port/mbedtls_debug.c"
                           "${COMPONENT_DIR}/port/esp_platform_time.c"
                           "${COMPONENT_DIR}/port/esp_timing.c")

list(APPEND mbedtls_target_sources "${COMPONENT_DIR}/port/esp_psa_crypto_init.c")
# Add ESP-IDF NVS-based PSA ITS implementation
# Only compile esp_psa_its.c if nvs_flash component is available
if(NOT ${IDF_TARGET} STREQUAL "linux")
    if(IDF_BUILD_V2)
    # For v2: conditionally compile source and link only if nvs_flash target exists
    target_sources(
        tfpsacrypto PRIVATE
        "$<$<TARGET_EXISTS:idf::nvs_flash>:${COMPONENT_DIR}/port/psa_crypto_storage/esp_psa_its.c>"
        )
        target_link_libraries(tfpsacrypto PRIVATE "$<$<TARGET_EXISTS:idf::nvs_flash>:idf::nvs_flash>")
        # Define compile definition to indicate ESP-IDF PSA ITS implementation is available
        target_compile_definitions(tfpsacrypto PUBLIC "$<$<TARGET_EXISTS:idf::nvs_flash>:ESP_PSA_ITS_AVAILABLE>")
    else()
        # For v1: check if component is in build before adding source and linking
        idf_build_get_property(build_components BUILD_COMPONENTS)
        if(nvs_flash IN_LIST build_components)
            target_sources(tfpsacrypto PRIVATE "${COMPONENT_DIR}/port/psa_crypto_storage/esp_psa_its.c")
            idf_component_get_property(nvs_flash_lib nvs_flash COMPONENT_LIB)
            target_link_libraries(tfpsacrypto PRIVATE ${nvs_flash_lib})
            target_compile_definitions(tfpsacrypto PUBLIC ESP_PSA_ITS_AVAILABLE)
        endif()
    endif()
endif()

# PSA Attestation
if(CONFIG_SECURE_TEE_ATTESTATION)
    target_sources(tfpsacrypto PRIVATE "${COMPONENT_DIR}/port/psa_attestation/psa_initial_attestation.c")
endif()

if(CONFIG_MBEDTLS_DYNAMIC_BUFFER)
    set(mbedtls_target_sources ${mbedtls_target_sources}
                           "${COMPONENT_DIR}/port/dynamic/esp_mbedtls_dynamic_impl.c"
                           "${COMPONENT_DIR}/port/dynamic/esp_ssl_cli.c"
                           "${COMPONENT_DIR}/port/dynamic/esp_ssl_srv.c"
                           "${COMPONENT_DIR}/port/dynamic/esp_ssl_tls.c")
endif()

if(${IDF_TARGET} STREQUAL "linux")
    set(mbedtls_target_sources ${mbedtls_target_sources} "${COMPONENT_DIR}/port/net_sockets.c")
endif()

# While updating to MbedTLS release/v3.4.0, building mbedtls/library/psa_crypto.c
# clang produces an unreachable-code warning.
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(tfpsacrypto PRIVATE "-Wno-unreachable-code")
endif()

# net_sockets.c should only be compiled if BSD socket functions are available.
# Do this by checking if lwip component is included into the build.
if(CONFIG_LWIP_ENABLE)
    if(IDF_BUILD_V2)
        target_sources(${COMPONENT_LIB} PRIVATE "$<$<TARGET_EXISTS:idf::lwip>:${COMPONENT_DIR}/port/net_sockets.c>")
        target_link_libraries(${COMPONENT_LIB} ${linkage_type} "$<$<TARGET_EXISTS:idf::lwip>:idf::lwip>")
    else()
        list(APPEND mbedtls_target_sources "${COMPONENT_DIR}/port/net_sockets.c")
        idf_component_get_property(lwip_lib lwip COMPONENT_LIB)
        target_link_libraries(${COMPONENT_LIB} ${linkage_type} ${lwip_lib})
    endif()
endif()

# Add port files to mbedtls targets
target_sources(mbedtls PRIVATE ${mbedtls_target_sources})

if(NOT ${IDF_TARGET} STREQUAL "linux")
    target_link_libraries(tfpsacrypto PUBLIC idf::esp_security)
    target_link_libraries(builtin PRIVATE idf::esp_security)
    target_link_libraries(p256m PRIVATE idf::esp_security)
endif()

# Choose peripheral type

if(CONFIG_SOC_SHA_SUPPORTED)
    if(CONFIG_SOC_SHA_SUPPORT_PARALLEL_ENG)
        set(SHA_PERIPHERAL_TYPE "parallel_engine")
    else()
        set(SHA_PERIPHERAL_TYPE "core")
    endif()
endif()

if(CONFIG_SOC_AES_SUPPORTED)
    if(CONFIG_SOC_AES_SUPPORT_DMA)
        set(AES_PERIPHERAL_TYPE "dma")
    else()
        set(AES_PERIPHERAL_TYPE "block")
    endif()
endif()

if(SHA_PERIPHERAL_TYPE STREQUAL "core")
    target_include_directories(tfpsacrypto PRIVATE "${COMPONENT_DIR}/port/sha/core/include")
    if(CONFIG_SOC_SHA_GDMA)
        set(SHA_CORE_SRCS "${COMPONENT_DIR}/port/sha/core/esp_sha_gdma_impl.c")
    elseif(CONFIG_SOC_SHA_CRYPTO_DMA)
        set(SHA_CORE_SRCS "${COMPONENT_DIR}/port/sha/core/esp_sha_crypto_dma_impl.c")
    endif()
    target_sources(tfpsacrypto PRIVATE  "${SHA_CORE_SRCS}")
endif()

if(AES_PERIPHERAL_TYPE STREQUAL "dma")
    if(NOT CONFIG_SOC_AES_GDMA)
        set(AES_DMA_SRCS "${COMPONENT_DIR}/port/aes/dma/esp_aes_crypto_dma_impl.c")
    else()
        set(AES_DMA_SRCS "${COMPONENT_DIR}/port/aes/dma/esp_aes_gdma_impl.c")
    endif()

    list(APPEND AES_DMA_SRCS "${COMPONENT_DIR}/port/aes/dma/esp_aes_dma_core.c")

    target_include_directories(tfpsacrypto PRIVATE "${COMPONENT_DIR}/port/aes/dma/include")
    target_sources(tfpsacrypto PRIVATE "${AES_DMA_SRCS}")
endif()

if(CONFIG_SOC_SHA_GDMA OR CONFIG_SOC_AES_GDMA)
    target_link_libraries(tfpsacrypto PRIVATE idf::esp_driver_dma)
endif()

if((SHA_PERIPHERAL_TYPE STREQUAL "core" AND CONFIG_SOC_SHA_SUPPORT_DMA) OR AES_PERIPHERAL_TYPE STREQUAL "dma")
    target_link_libraries(tfpsacrypto PRIVATE idf::esp_mm)
    target_link_libraries(builtin PRIVATE idf::esp_mm)
    if(CONFIG_SOC_SHA_GDMA OR CONFIG_SOC_AES_GDMA)
        if(CONFIG_SOC_AXI_DMA_EXT_MEM_ENC_ALIGNMENT)
            target_link_libraries(tfpsacrypto PRIVATE idf::bootloader_support)
            target_link_libraries(builtin PRIVATE idf::bootloader_support)
        endif()
        target_sources(tfpsacrypto PRIVATE "${COMPONENT_DIR}/port/crypto_shared_gdma/esp_crypto_shared_gdma.c")
    endif()
endif()

if(NOT ${IDF_TARGET} STREQUAL "linux")
    target_sources(tfpsacrypto PRIVATE "${COMPONENT_DIR}/port/esp_hardware.c")
endif()
target_sources(tfpsacrypto PRIVATE "${COMPONENT_DIR}/port/esp_mem.c")

if(CONFIG_SOC_AES_SUPPORTED)
    target_include_directories(tfpsacrypto PRIVATE "${COMPONENT_DIR}/port/aes/include")
    target_sources(tfpsacrypto PRIVATE "${COMPONENT_DIR}/port/aes/esp_aes_xts.c"
                                  "${COMPONENT_DIR}/port/aes/esp_aes_common.c"
                                  "${COMPONENT_DIR}/port/aes/esp_aes.c"
                                  "${COMPONENT_DIR}/port/aes/esp_aes_gcm.c"
    )
endif()

if(CONFIG_SOC_SHA_SUPPORTED)
    if(CONFIG_MBEDTLS_HARDWARE_SHA)
        target_sources(tfpsacrypto PRIVATE
            "${COMPONENT_DIR}/port/psa_driver/esp_sha/psa_crypto_driver_esp_sha.c"
            "${COMPONENT_DIR}/port/psa_driver/esp_sha/${SHA_PERIPHERAL_TYPE}/psa_crypto_driver_esp_sha256.c"
            "${COMPONENT_DIR}/port/psa_driver/esp_sha/${SHA_PERIPHERAL_TYPE}/psa_crypto_driver_esp_sha512.c"
            "${COMPONENT_DIR}/port/sha/esp_sha.c"
            "${COMPONENT_DIR}/port/psa_driver/esp_mac/psa_crypto_driver_esp_hmac_transparent.c"
        )
    endif()
    target_sources(tfpsacrypto PRIVATE
            "${COMPONENT_DIR}/port/psa_driver/esp_sha/${SHA_PERIPHERAL_TYPE}/psa_crypto_driver_esp_sha1.c"
            "${COMPONENT_DIR}/port/sha/${SHA_PERIPHERAL_TYPE}/sha.c"
    )
endif()

if(CONFIG_MBEDTLS_ROM_MD5)
    target_sources(tfpsacrypto PRIVATE
        "${COMPONENT_DIR}/port/psa_driver/esp_md/psa_crypto_driver_esp_md5.c"
    )
endif()

if(CONFIG_SOC_HMAC_SUPPORTED)
    target_sources(tfpsacrypto PRIVATE
        "${COMPONENT_DIR}/port/psa_driver/esp_mac/psa_crypto_driver_esp_hmac_opaque.c"
    )
    target_link_libraries(tfpsacrypto PRIVATE idf::efuse)
endif()

if(CONFIG_SOC_DIG_SIGN_SUPPORTED AND CONFIG_MBEDTLS_HARDWARE_RSA_DS_PERIPHERAL)
target_sources(tfpsacrypto PRIVATE
                "${COMPONENT_DIR}/port/psa_driver/esp_rsa_ds/psa_crypto_driver_esp_rsa_ds.c"
                "${COMPONENT_DIR}/port/psa_driver/esp_rsa_ds/psa_crypto_driver_esp_rsa_ds_utilities.c"
)
target_link_libraries(tfpsacrypto PRIVATE idf::efuse)
endif()

if(CONFIG_SOC_HMAC_SUPPORTED)
    target_sources(tfpsacrypto PRIVATE "${COMPONENT_DIR}/port/esp_hmac_pbkdf2.c")
endif()

# Note: some mbedTLS hardware acceleration can be enabled/disabled by config.
#
# We don't need to filter aes.c as this uses a different prefix (esp_aes_x) and the
# config option only changes the prefixes in the header so mbedtls_aes_x compiles to esp_aes_x
#
# The other port-specific files don't override internal mbedTLS functions, they just add new functions.

if(CONFIG_MBEDTLS_HARDWARE_MPI)
    target_sources(tfpsacrypto PRIVATE  "${COMPONENT_DIR}/port/bignum/esp_bignum.c"
                                       "${COMPONENT_DIR}/port/bignum/bignum_alt.c")
endif()

if(CONFIG_MBEDTLS_HARDWARE_GCM OR CONFIG_MBEDTLS_HARDWARE_AES)
    target_include_directories(tfpsacrypto PRIVATE "${COMPONENT_DIR}/port/include/aes")
    target_sources(tfpsacrypto PRIVATE
        "${COMPONENT_DIR}/port/psa_driver/esp_aes/psa_crypto_driver_esp_aes.c"
        "${COMPONENT_DIR}/port/psa_driver/esp_aes/psa_crypto_driver_esp_aes_gcm.c"
    )
    if(CONFIG_MBEDTLS_CMAC_C)
        target_sources(tfpsacrypto PRIVATE
            "${COMPONENT_DIR}/port/psa_driver/esp_mac/psa_crypto_driver_esp_cmac.c"
        )
    endif()
endif()

if(CONFIG_SOC_ECC_SUPPORTED)
    target_sources(builtin PRIVATE "${COMPONENT_DIR}/port/ecc/esp_ecc.c")
    if(CONFIG_MBEDTLS_HARDWARE_ECC)
        target_sources(builtin PRIVATE "${COMPONENT_DIR}/port/ecc/ecc_alt.c")
        include_directories("${COMPONENT_DIR}/tf-psa-crypto/drivers/builtin/include/mbedtls")
    endif()
endif()

if(CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN OR CONFIG_MBEDTLS_HARDWARE_ECDSA_VERIFY OR CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN)
    target_sources(tfpsacrypto PRIVATE "${COMPONENT_DIR}/port/psa_driver/esp_ecdsa/psa_crypto_driver_esp_ecdsa.c")
    target_include_directories(tfpsacrypto PUBLIC "${COMPONENT_DIR}/port/psa_driver/include")
    target_link_libraries(tfpsacrypto PRIVATE idf::efuse)
    if(CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN)
        target_link_libraries(tfpsacrypto PRIVATE idf::tee_sec_storage)
    endif()
endif()

# if(CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL)
#     target_sources(mbedcrypto PRIVATE  "${COMPONENT_DIR}/port/mbedtls_rom/mbedtls_rom_osi.c")
#     target_link_libraries(${COMPONENT_LIB} PRIVATE "-u mbedtls_rom_osi_functions_init")
# endif()

if(CONFIG_COMPILER_STATIC_ANALYZER AND CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${COMPONENT_LIB} PRIVATE "-fno-analyzer")
    target_compile_options(tfpsacrypto PRIVATE "-fno-analyzer")
endif()

# If linkage_type is PUBLIC, use PRIVATE while setting compiler optimization flags
# as we don't want the optimization flags to modify other targets
if(linkage_type STREQUAL "PUBLIC")
    set(compiler_linkage_type PRIVATE)
else()
    set(compiler_linkage_type ${linkage_type})
endif()

if(CONFIG_MBEDTLS_COMPILER_OPTIMIZATION_SIZE)
    message(STATUS "Linkage type is ${linkage_type}")
    target_compile_options(${COMPONENT_LIB} ${compiler_linkage_type} "-Os")
elseif(CONFIG_MBEDTLS_COMPILER_OPTIMIZATION_PERF)
    target_compile_options(${COMPONENT_LIB} ${compiler_linkage_type} "-O2")
endif()

if(CONFIG_MBEDTLS_DYNAMIC_BUFFER)
    set(WRAP_FUNCTIONS
        mbedtls_ssl_write_client_hello
        mbedtls_ssl_handshake_client_step
        mbedtls_ssl_tls13_handshake_client_step
        mbedtls_ssl_handshake_server_step
        mbedtls_ssl_read
        mbedtls_ssl_write
        mbedtls_ssl_free
        mbedtls_ssl_setup
        mbedtls_ssl_send_alert_message
        mbedtls_ssl_close_notify)

    foreach(wrap ${WRAP_FUNCTIONS})
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=${wrap}")
    endforeach()
endif()

# set_property(TARGET mbedcrypto APPEND PROPERTY LINK_INTERFACE_LIBRARIES mbedtls)

if(CONFIG_PM_ENABLE)
    target_link_libraries(tfpsacrypto PRIVATE idf::esp_pm)
endif()

target_link_libraries(${COMPONENT_LIB} ${linkage_type} ${mbedtls_targets})

# Ensure PSA crypto initialization is included in the build
if(NOT ${IDF_TARGET} STREQUAL "linux")
    target_link_libraries(${COMPONENT_LIB} ${linkage_type} "-u mbedtls_psa_crypto_init_include_impl")
endif()

# Additional optional dependencies for the mbedcrypto library
function(builtin_optional_deps component_name)
    idf_build_get_property(components BUILD_COMPONENTS)
    if(${component_name} IN_LIST components)
        idf_component_get_property(lib_name ${component_name} COMPONENT_LIB)
        target_link_libraries(builtin PRIVATE ${lib_name})
    endif()
endfunction()

if(CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_CONSTANT_TIME_CM)
    target_link_libraries(tfpsacrypto PRIVATE idf::esp_timer)
endif()

# # Link esp-cryptoauthlib to mbedtls
# if(CONFIG_ATCA_MBEDTLS_ECDSA)
#     mbedcrypto_optional_deps(espressif__esp-cryptoauthlib esp-cryptoauthlib)
# endif()

# Apply -fno-analyzer to ALL mbedTLS targets at the very end when all targets are created
if(CONFIG_COMPILER_STATIC_ANALYZER AND CMAKE_C_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Applying -fno-analyzer to all mbedTLS targets...")

    # Get all targets from all directories
    get_property(
        all_mbedtls_targets DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls PROPERTY BUILDSYSTEM_TARGETS
    )
    get_property(
        drivers_targets DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls/tf-psa-crypto/drivers PROPERTY BUILDSYSTEM_TARGETS
    )

    message(STATUS "Found mbedtls targets: ${all_mbedtls_targets}")
    message(STATUS "Found drivers targets: ${drivers_targets}")

    # Get targets from nested driver subdirectories
    foreach(subdir IN ITEMS builtin everest p256-m)
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls/tf-psa-crypto/drivers/${subdir})
            get_property(
                subdir_targets DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls/tf-psa-crypto/drivers/${subdir}
                PROPERTY BUILDSYSTEM_TARGETS
            )
            message(STATUS "Found ${subdir} targets: ${subdir_targets}")
            list(APPEND drivers_targets ${subdir_targets})
        endif()
    endforeach()

    # Combine all target lists
    set(all_targets ${all_mbedtls_targets} ${drivers_targets})
    message(STATUS "All combined targets: ${all_targets}")

    # Apply -fno-analyzer to each target
    foreach(target ${all_targets})
        if(TARGET ${target})
            get_target_property(target_type ${target} TYPE)
            if(target_type STREQUAL "STATIC_LIBRARY" OR
               target_type STREQUAL "SHARED_LIBRARY" OR
               target_type STREQUAL "MODULE_LIBRARY" OR
               target_type STREQUAL "OBJECT_LIBRARY" OR
               target_type STREQUAL "EXECUTABLE")
                message(STATUS "Applying -fno-analyzer to target: ${target}")
                target_compile_options(${target} PRIVATE "-fno-analyzer")
            endif()
        endif()
    endforeach()

    # Also check for any targets that might have been missed by using global target list
    get_property(global_targets GLOBAL PROPERTY TARGETS)
    set(mbedtls_global_targets "")
    foreach(target ${global_targets})
        if(TARGET ${target})
            get_target_property(target_source_dir ${target} SOURCE_DIR)
            if(target_source_dir)
                # Check if target is from mbedtls directory or has mbedtls-related names
                string(FIND "${target_source_dir}" "mbedtls" pos)
                string(FIND "${target}" "mbedtls" name_pos)
                string(FIND "${target}" "tfpsacrypto" tfpsa_pos)
                # string(FIND "${target}" "everest" everest_pos)
                # string(FIND "${target}" "p256m" p256m_pos)
                string(FIND "${target}" "builtin" builtin_pos)
                if(pos GREATER -1 OR name_pos GREATER -1 OR tfpsa_pos GREATER -1 OR builtin_pos GREATER -1)
                    list(APPEND mbedtls_global_targets ${target})
                    get_target_property(target_type ${target} TYPE)
                    # Skip ALIAS targets as they don't have compile options
                    if(NOT target_type STREQUAL "ALIAS" AND
                       (target_type STREQUAL "STATIC_LIBRARY" OR
                        target_type STREQUAL "SHARED_LIBRARY" OR
                        target_type STREQUAL "MODULE_LIBRARY" OR
                        target_type STREQUAL "OBJECT_LIBRARY" OR
                        target_type STREQUAL "EXECUTABLE"))
                        # Check if -fno-analyzer was already applied
                        get_target_property(compile_options ${target} COMPILE_OPTIONS)
                        if(NOT compile_options OR NOT "-fno-analyzer" IN_LIST compile_options)
                            message(STATUS "Applying -fno-analyzer to missed target: ${target}")
                            target_compile_options(${target} PRIVATE "-fno-analyzer")
                        endif()
                    endif()
                endif()
            endif()
        endif()
    endforeach()
    message(STATUS "All mbedtls-related global targets: ${mbedtls_global_targets}")
endif()
