idf_build_get_property(idf_target IDF_TARGET)
idf_build_get_property(python PYTHON)

set(priv_requires soc esp_hw_support)

if(NOT BOOTLOADER_BUILD)
    list(APPEND priv_requires esp_pm)
endif()

idf_component_register(SRCS "esp_crt_bundle/esp_crt_bundle.c"
    INCLUDE_DIRS "port/include" "mbedtls/include" "esp_crt_bundle/include"
    REQUIRES lwip
    PRIV_REQUIRES "${priv_requires}"
    )

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

    if(CONFIG_MBEDTLS_CUSTOM_CERTIFICATE_BUNDLE)
        get_filename_component(custom_bundle_path
        ${CONFIG_MBEDTLS_CUSTOM_CERTIFICATE_BUNDLE_PATH} ABSOLUTE BASE_DIR "${project_dir}")
        list(APPEND crt_paths ${custom_bundle_path})

    endif()
    list(APPEND args --input ${crt_paths} -q)

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
        APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
        "${crt_bundle}")
endif()


# Only build mbedtls libraries
set(ENABLE_TESTING CACHE BOOL OFF)
set(ENABLE_PROGRAMS CACHE BOOL OFF)

# Needed to for include_next includes to work from within mbedtls
include_directories("${COMPONENT_DIR}/port/include")

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

set(mbedtls_targets mbedtls mbedcrypto mbedx509)

set(mbedtls_target_sources "${COMPONENT_DIR}/port/mbedtls_debug.c"
                           "${COMPONENT_DIR}/port/net_sockets.c")

if(CONFIG_MBEDTLS_DYNAMIC_BUFFER)
set(mbedtls_target_sources ${mbedtls_target_sources}
                           "${COMPONENT_DIR}/port/dynamic/esp_mbedtls_dynamic_impl.c"
                           "${COMPONENT_DIR}/port/dynamic/esp_ssl_cli.c"
                           "${COMPONENT_DIR}/port/dynamic/esp_ssl_srv.c"
                           "${COMPONENT_DIR}/port/dynamic/esp_ssl_tls.c")
endif()

# Add port files to mbedtls targets
target_sources(mbedtls PRIVATE ${mbedtls_target_sources})

# Choose perihperal type
if(CONFIG_IDF_TARGET_ESP32)
    set(SHA_PERIPHERAL_TYPE "parallel_engine")
    set(AES_PERIPHERAL_TYPE "block")
else()
    set(SHA_PERIPHERAL_TYPE "dma")
    set(AES_PERIPHERAL_TYPE "dma")
endif()

if(SHA_PERIPHERAL_TYPE STREQUAL "dma")
    target_include_directories(mbedcrypto PRIVATE "${COMPONENT_DIR}/port/sha/dma/include")

    if(CONFIG_IDF_TARGET_ESP32S2)
        set(SHA_DMA_SRCS "${COMPONENT_DIR}/port/sha/dma/esp_sha_crypto_dma_impl.c")
    else()
        set(SHA_DMA_SRCS "${COMPONENT_DIR}/port/sha/dma/esp_sha_gdma_impl.c"
                         "${COMPONENT_DIR}/port/crypto_shared_gdma/esp_crypto_shared_gdma.c")
    endif()
    target_sources(mbedcrypto PRIVATE  "${SHA_DMA_SRCS}")
endif()

if(AES_PERIPHERAL_TYPE STREQUAL "dma")

    if(CONFIG_IDF_TARGET_ESP32S2)
        set(AES_DMA_SRCS "${COMPONENT_DIR}/port/aes/dma/esp_aes_crypto_dma_impl.c")
    else()
        set(AES_DMA_SRCS "${COMPONENT_DIR}/port/aes/dma/esp_aes_gdma_impl.c")
    endif()

    target_include_directories(mbedcrypto PRIVATE "${COMPONENT_DIR}/port/aes/dma/include")
    target_sources(mbedcrypto PRIVATE  "${AES_DMA_SRCS}")
endif()

target_sources(mbedcrypto PRIVATE "${COMPONENT_DIR}/port/esp_hardware.c"
                                  "${COMPONENT_DIR}/port/esp_mem.c"
                                  "${COMPONENT_DIR}/port/esp_timing.c"
                                  "${COMPONENT_DIR}/port/sha/esp_sha.c"
                                  "${COMPONENT_DIR}/port/aes/esp_aes_xts.c"
                                  "${COMPONENT_DIR}/port/aes/esp_aes_common.c"
                                  "${COMPONENT_DIR}/port/aes/${AES_PERIPHERAL_TYPE}/esp_aes.c"
                                  "${COMPONENT_DIR}/port/sha/${SHA_PERIPHERAL_TYPE}/sha.c"
)

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
    target_sources(mbedcrypto PRIVATE  "${COMPONENT_DIR}/port/esp_bignum.c"
                                       "${COMPONENT_DIR}/port/${idf_target}/bignum.c"
    )
endif()

if(CONFIG_MBEDTLS_HARDWARE_SHA)
    target_sources(mbedcrypto PRIVATE  "${COMPONENT_DIR}/port/sha/${SHA_PERIPHERAL_TYPE}/esp_sha1.c"
                                       "${COMPONENT_DIR}/port/sha/${SHA_PERIPHERAL_TYPE}/esp_sha256.c"
                                       "${COMPONENT_DIR}/port/sha/${SHA_PERIPHERAL_TYPE}/esp_sha512.c"
    )
endif()

if(CONFIG_MBEDTLS_HARDWARE_GCM)
    target_sources(mbedcrypto PRIVATE  "${COMPONENT_DIR}/port/aes/esp_aes_gcm.c")
endif()

if(CONFIG_MBEDTLS_ROM_MD5)
    target_sources(mbedcrypto PRIVATE  "${COMPONENT_DIR}/port/md/esp_md.c")
endif()

foreach(target ${mbedtls_targets})
    target_compile_definitions(${target} PUBLIC -DMBEDTLS_CONFIG_FILE="mbedtls/esp_config.h")
endforeach()

if(CONFIG_MBEDTLS_DYNAMIC_BUFFER)
    set(WRAP_FUNCTIONS
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
set_property(TARGET mbedcrypto APPEND PROPERTY LINK_LIBRARIES idf::driver idf::${target})
set_property(TARGET mbedcrypto APPEND PROPERTY INTERFACE_LINK_LIBRARIES idf::driver idf::${target})

# Link mbedtls libraries to component library
target_link_libraries(${COMPONENT_LIB} PUBLIC ${mbedtls_targets})

if(CONFIG_ESP_TLS_USE_DS_PERIPHERAL)
    # Link target (e.g. esp32s2) library to component library
    idf_component_get_property(target_lib ${target} COMPONENT_LIB)
    set_property(TARGET mbedcrypto APPEND PROPERTY INTERFACE_LINK_LIBRARIES $<LINK_ONLY:${target_lib}>)
    # The linker seems to be unable to resolve all the dependencies without increasing this
    set_property(TARGET mbedcrypto APPEND PROPERTY LINK_INTERFACE_MULTIPLICITY 6)
    target_link_libraries(${COMPONENT_LIB} PUBLIC ${target_lib})
endif()

# Link esp-cryptoauthlib to mbedtls
if(CONFIG_ATCA_MBEDTLS_ECDSA)
    idf_component_get_property(cryptoauthlib esp-cryptoauthlib COMPONENT_LIB)
    target_link_libraries(${COMPONENT_LIB} PUBLIC ${cryptoauthlib})
endif()
