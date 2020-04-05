idf_build_get_property(idf_target IDF_TARGET)

idf_component_register(SRCS "esp_crt_bundle/esp_crt_bundle.c"
    INCLUDE_DIRS "port/include" "mbedtls/include" "esp_crt_bundle/include"
    REQUIRES lwip
    PRIV_REQUIRES soc
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

set(mbedtls_targets mbedtls mbedcrypto mbedx509)

# Add port files to mbedtls targets
target_sources(mbedtls PRIVATE  "${COMPONENT_DIR}/port/mbedtls_debug.c"
                                "${COMPONENT_DIR}/port/net_sockets.c")



target_sources(mbedcrypto PRIVATE "${COMPONENT_DIR}/port/esp_hardware.c"
                                "${COMPONENT_DIR}/port/esp_mem.c"
                                "${COMPONENT_DIR}/port/esp_timing.c"
                                "${COMPONENT_DIR}/port/esp_sha.c"
				"${COMPONENT_DIR}/port/esp_bignum.c"
                                "${COMPONENT_DIR}/port/esp_aes_xts.c"
                                "${COMPONENT_DIR}/port/${idf_target}/bignum.c"
                                "${COMPONENT_DIR}/port/${idf_target}/aes.c"
                                "${COMPONENT_DIR}/port/${idf_target}/sha.c"
                                "${COMPONENT_DIR}/port/${idf_target}/esp_sha1.c"
                                "${COMPONENT_DIR}/port/${idf_target}/esp_sha256.c"
                                "${COMPONENT_DIR}/port/${idf_target}/esp_sha512.c")

foreach(target ${mbedtls_targets})
    target_compile_definitions(${target} PUBLIC -DMBEDTLS_CONFIG_FILE="mbedtls/esp_config.h")
endforeach()

set_property(TARGET mbedcrypto APPEND PROPERTY LINK_INTERFACE_LIBRARIES mbedtls)

# Link mbedtls libraries to component library
target_link_libraries(${COMPONENT_LIB} INTERFACE ${mbedtls_targets})


