idf_build_get_property(idf_target IDF_TARGET)
idf_build_get_property(python PYTHON)

if(NOT ${IDF_TARGET} STREQUAL "linux")
set(priv_requires soc esp_hw_support)

if(NOT BOOTLOADER_BUILD)
    list(APPEND priv_requires esp_pm)
endif()
endif()

set(mbedtls_srcs "")
set(mbedtls_include_dirs "port/include" "mbedtls/include" "mbedtls/library")

if(CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL)
    list(APPEND mbedtls_include_dirs "port/mbedtls_rom")
endif()

if(CONFIG_MBEDTLS_CERTIFICATE_BUNDLE)
    list(APPEND mbedtls_srcs "esp_crt_bundle/esp_crt_bundle.c")
    list(APPEND mbedtls_include_dirs "esp_crt_bundle/include")
endif()

idf_component_register(SRCS "${mbedtls_srcs}"
    INCLUDE_DIRS "${mbedtls_include_dirs}"
    PRIV_REQUIRES "${priv_requires}"
    )

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
        list(APPEND crt_paths ${DEFAULT_CRT_DIR}/cacrt_all.pem ${DEFAULT_CRT_DIR}/cacrt_local.pem)
    elseif(CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEFAULT_CMN)
        list(APPEND crt_paths ${DEFAULT_CRT_DIR}/cacrt_all.pem ${DEFAULT_CRT_DIR}/cacrt_local.pem)
        list(APPEND args --filter ${DEFAULT_CRT_DIR}/cmn_crt_authorities.csv)
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
set(ENABLE_TESTING CACHE BOOL OFF)
set(ENABLE_PROGRAMS CACHE BOOL OFF)

# Use pre-generated source files in mbedtls repository
set(GEN_FILES CACHE BOOL OFF)

# Make sure mbedtls finds the same Python interpreter as IDF uses
idf_build_get_property(python PYTHON)
set(Python3_EXECUTABLE ${python})

# Needed to for include_next includes to work from within mbedtls
set(include_dirs "${COMPONENT_DIR}/port/include")

if(CONFIG_MBEDTLS_CERTIFICATE_BUNDLE)
    list(APPEND include_dirs "${COMPONENT_DIR}/esp_crt_bundle/include")
endif()

include_directories(${include_dirs})

# Needed to for mbedtls_rom includes to work from within mbedtls
if(CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL)
    include_directories("${COMPONENT_DIR}/port/mbedtls_rom")
endif()

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

get_target_property(src_crypto mbedcrypto SOURCES)
list(REMOVE_ITEM src_crypto cipher_wrap.c ecdsa.c ecp.c ecp_curves.c oid.c pk_wrap.c)
set_property(TARGET mbedcrypto PROPERTY SOURCES ${src_crypto})

get_target_property(src_x509 mbedx509 SOURCES)
list(REMOVE_ITEM src_x509 x509_crt.c)
set_property(TARGET mbedx509 PROPERTY SOURCES ${src_x509})
endif()

# Core libraries from the mbedTLS project
set(mbedtls_targets mbedtls mbedcrypto mbedx509)
# 3rd party libraries from the mbedTLS project
list(APPEND mbedtls_targets everest p256m)

set(mbedtls_target_sources "${COMPONENT_DIR}/port/mbedtls_debug.c"
                           "${COMPONENT_DIR}/port/esp_platform_time.c")

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
    target_compile_options(mbedcrypto PRIVATE "-Wno-unreachable-code")
endif()

# net_sockets.c should only be compiled if BSD socket functions are available.
# Do this by checking if lwip component is included into the build.
if(CONFIG_LWIP_ENABLE)
    list(APPEND mbedtls_target_sources "${COMPONENT_DIR}/port/net_sockets.c")
    idf_component_get_property(lwip_lib lwip COMPONENT_LIB)
    target_link_libraries(${COMPONENT_LIB} ${linkage_type} ${lwip_lib})
endif()

# Add port files to mbedtls targets
target_sources(mbedtls PRIVATE ${mbedtls_target_sources})

if(NOT ${IDF_TARGET} STREQUAL "linux")
    target_link_libraries(mbedcrypto PRIVATE idf::esp_security)
endif()

# Choose peripheral type

if(CONFIG_SOC_SHA_SUPPORTED)
    if(CONFIG_SOC_SHA_SUPPORT_DMA)
        set(SHA_PERIPHERAL_TYPE "dma")
    elseif(CONFIG_SOC_SHA_SUPPORT_PARALLEL_ENG)
        set(SHA_PERIPHERAL_TYPE "parallel_engine")
    else()
        set(SHA_PERIPHERAL_TYPE "block")
    endif()
endif()

if(CONFIG_SOC_AES_SUPPORTED)
    if(CONFIG_SOC_AES_SUPPORT_DMA)
        set(AES_PERIPHERAL_TYPE "dma")
    else()
        set(AES_PERIPHERAL_TYPE "block")
    endif()
endif()

if(SHA_PERIPHERAL_TYPE STREQUAL "dma")
    target_include_directories(mbedcrypto PRIVATE "${COMPONENT_DIR}/port/sha/dma/include")

    if(NOT CONFIG_SOC_SHA_GDMA)
        set(SHA_DMA_SRCS "${COMPONENT_DIR}/port/sha/dma/esp_sha_crypto_dma_impl.c")
    else()
        set(SHA_DMA_SRCS "${COMPONENT_DIR}/port/sha/dma/esp_sha_gdma_impl.c")
    endif()
    target_sources(mbedcrypto PRIVATE  "${SHA_DMA_SRCS}")
endif()

if(AES_PERIPHERAL_TYPE STREQUAL "dma")
    if(NOT CONFIG_SOC_AES_GDMA)
        set(AES_DMA_SRCS "${COMPONENT_DIR}/port/aes/dma/esp_aes_crypto_dma_impl.c")
    else()
        set(AES_DMA_SRCS "${COMPONENT_DIR}/port/aes/dma/esp_aes_gdma_impl.c")
    endif()

    list(APPEND AES_DMA_SRCS "${COMPONENT_DIR}/port/aes/dma/esp_aes_dma_core.c")

    target_include_directories(mbedcrypto PRIVATE "${COMPONENT_DIR}/port/aes/dma/include")
    target_sources(mbedcrypto PRIVATE  "${AES_DMA_SRCS}")
endif()

if(SHA_PERIPHERAL_TYPE STREQUAL "dma" OR AES_PERIPHERAL_TYPE STREQUAL "dma")
    target_link_libraries(mbedcrypto PRIVATE idf::esp_mm)
    if(CONFIG_SOC_SHA_GDMA OR CONFIG_SOC_AES_GDMA)
        if(CONFIG_SOC_AXI_DMA_EXT_MEM_ENC_ALIGNMENT)
            target_link_libraries(mbedcrypto PRIVATE idf::bootloader_support)
        endif()
        target_sources(mbedcrypto PRIVATE "${COMPONENT_DIR}/port/crypto_shared_gdma/esp_crypto_shared_gdma.c")
    endif()
endif()

if(NOT ${IDF_TARGET} STREQUAL "linux")
    target_sources(mbedcrypto PRIVATE "${COMPONENT_DIR}/port/esp_hardware.c")
endif()
target_sources(mbedcrypto PRIVATE "${COMPONENT_DIR}/port/esp_mem.c"
                                  "${COMPONENT_DIR}/port/esp_timing.c"
)

if(CONFIG_SOC_AES_SUPPORTED)
    target_include_directories(mbedcrypto PRIVATE "${COMPONENT_DIR}/port/aes/include")
    target_sources(mbedcrypto PRIVATE "${COMPONENT_DIR}/port/aes/esp_aes_xts.c"
                                  "${COMPONENT_DIR}/port/aes/esp_aes_common.c"
                                  "${COMPONENT_DIR}/port/aes/${AES_PERIPHERAL_TYPE}/esp_aes.c"
    )
endif()

if(CONFIG_SOC_SHA_SUPPORTED)
    target_sources(mbedcrypto PRIVATE "${COMPONENT_DIR}/port/sha/esp_sha.c"
                                    "${COMPONENT_DIR}/port/sha/${SHA_PERIPHERAL_TYPE}/sha.c"
    )
endif()

# CONFIG_ESP_TLS_USE_DS_PERIPHERAL can be enabled only for the supported targets.
if(CONFIG_ESP_TLS_USE_DS_PERIPHERAL)
    target_sources(mbedcrypto PRIVATE "${COMPONENT_DIR}/port/esp_ds/esp_rsa_sign_alt.c")
endif()

# Note: some mbedTLS hardware acceleration can be enabled/disabled by config.
#
# We don't need to filter aes.c as this uses a different prefix (esp_aes_x) and the
# config option only changes the prefixes in the header so mbedtls_aes_x compiles to esp_aes_x
#
# The other port-specific files don't override internal mbedTLS functions, they just add new functions.

if(CONFIG_MBEDTLS_HARDWARE_MPI)
    target_sources(mbedcrypto PRIVATE  "${COMPONENT_DIR}/port/bignum/esp_bignum.c"
                                       "${COMPONENT_DIR}/port/bignum/bignum_alt.c")
endif()

if(CONFIG_MBEDTLS_HARDWARE_SHA)
    target_sources(mbedcrypto PRIVATE  "${COMPONENT_DIR}/port/sha/${SHA_PERIPHERAL_TYPE}/esp_sha1.c"
                                       "${COMPONENT_DIR}/port/sha/${SHA_PERIPHERAL_TYPE}/esp_sha256.c"
                                       "${COMPONENT_DIR}/port/sha/${SHA_PERIPHERAL_TYPE}/esp_sha512.c"
    )
endif()

if(CONFIG_MBEDTLS_HARDWARE_GCM OR CONFIG_MBEDTLS_HARDWARE_AES)
    target_sources(mbedcrypto PRIVATE  "${COMPONENT_DIR}/port/aes/esp_aes_gcm.c")
endif()

if(CONFIG_MBEDTLS_HARDWARE_ECC)
    target_sources(mbedcrypto PRIVATE  "${COMPONENT_DIR}/port/ecc/esp_ecc.c"
                                       "${COMPONENT_DIR}/port/ecc/ecc_alt.c")
endif()

if(CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN OR CONFIG_MBEDTLS_HARDWARE_ECDSA_VERIFY)
    target_sources(mbedcrypto PRIVATE  "${COMPONENT_DIR}/port/ecdsa/ecdsa_alt.c")

    if(CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN)
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=mbedtls_ecdsa_sign")
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=mbedtls_ecdsa_sign_restartable")
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=mbedtls_ecdsa_write_signature")
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=mbedtls_ecdsa_write_signature_restartable")

        if(CONFIG_SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE)
            target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=mbedtls_ecdsa_sign_det_ext")
            target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=mbedtls_ecdsa_sign_det_restartable")
        endif()

    endif()

    if(CONFIG_MBEDTLS_HARDWARE_ECDSA_VERIFY)
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=mbedtls_ecdsa_verify")
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=mbedtls_ecdsa_verify_restartable")
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=mbedtls_ecdsa_read_signature")
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=mbedtls_ecdsa_read_signature_restartable")
    endif()
endif()

if(CONFIG_MBEDTLS_ROM_MD5)
    target_sources(mbedcrypto PRIVATE  "${COMPONENT_DIR}/port/md/esp_md.c")
endif()

if(CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL)
    target_sources(mbedcrypto PRIVATE  "${COMPONENT_DIR}/port/mbedtls_rom/mbedtls_rom_osi.c")
    target_link_libraries(${COMPONENT_LIB} PRIVATE "-u mbedtls_rom_osi_functions_init")
endif()

foreach(target ${mbedtls_targets})
    target_compile_definitions(${target} PUBLIC -DMBEDTLS_CONFIG_FILE="mbedtls/esp_config.h")
    if(CONFIG_COMPILER_STATIC_ANALYZER AND CMAKE_C_COMPILER_ID STREQUAL "GNU") # TODO IDF-10087
        target_compile_options(${target} PRIVATE "-fno-analyzer")
    endif()
endforeach()

if(CONFIG_MBEDTLS_DYNAMIC_BUFFER)
    set(WRAP_FUNCTIONS
        mbedtls_ssl_write_client_hello
        mbedtls_ssl_handshake_client_step
        mbedtls_ssl_handshake_server_step
        mbedtls_ssl_read
        mbedtls_ssl_write
        mbedtls_ssl_session_reset
        mbedtls_ssl_free
        mbedtls_ssl_setup
        mbedtls_ssl_send_alert_message
        mbedtls_ssl_close_notify)

    foreach(wrap ${WRAP_FUNCTIONS})
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=${wrap}")
    endforeach()
endif()

set_property(TARGET mbedcrypto APPEND PROPERTY LINK_INTERFACE_LIBRARIES mbedtls)

if(CONFIG_PM_ENABLE)
    target_link_libraries(mbedcrypto PRIVATE idf::esp_pm)
endif()

if(CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN OR CONFIG_MBEDTLS_HARDWARE_ECDSA_VERIFY)
    target_link_libraries(mbedcrypto PRIVATE idf::efuse)
endif()

target_link_libraries(${COMPONENT_LIB} ${linkage_type} ${mbedtls_targets})

if(CONFIG_ESP_TLS_USE_DS_PERIPHERAL)
    # The linker seems to be unable to resolve all the dependencies without increasing this
    set_property(TARGET mbedcrypto APPEND PROPERTY LINK_INTERFACE_MULTIPLICITY 6)
endif()

# Additional optional dependencies for the mbedcrypto library
function(mbedcrypto_optional_deps component_name)
    idf_build_get_property(components BUILD_COMPONENTS)
    if(${component_name} IN_LIST components)
        idf_component_get_property(lib_name ${component_name} COMPONENT_LIB)
        target_link_libraries(mbedcrypto PRIVATE ${lib_name})
    endif()
endfunction()

if(CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_CONSTANT_TIME_CM)
    mbedcrypto_optional_deps(esp_timer idf::esp_timer)
endif()

# Link esp-cryptoauthlib to mbedtls
if(CONFIG_ATCA_MBEDTLS_ECDSA)
    mbedcrypto_optional_deps(espressif__esp-cryptoauthlib esp-cryptoauthlib)
endif()
