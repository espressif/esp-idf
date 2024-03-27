set(srcs esp_tls.c esp-tls-crypto/esp_tls_crypto.c esp_tls_error_capture.c)
if(CONFIG_ESP_TLS_USING_MBEDTLS)
    list(APPEND srcs
        "esp_tls_mbedtls.c")
endif()

if(CONFIG_ESP_TLS_USING_WOLFSSL)
    list(APPEND srcs
        "esp_tls_wolfssl.c")
endif()

set(priv_req http_parser)
if(NOT ${IDF_TARGET} STREQUAL "linux")
    list(APPEND priv_req lwip)
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} esp-tls-crypto
                    PRIV_INCLUDE_DIRS "private_include"
                    # mbedtls is public requirements becasue esp_tls.h
                    # includes mbedtls header files.
                    REQUIRES mbedtls
                    PRIV_REQUIRES ${priv_req})

if(CONFIG_ESP_TLS_USING_WOLFSSL)
    idf_component_get_property(wolfssl esp-wolfssl COMPONENT_LIB)
    target_link_libraries(${COMPONENT_LIB} PUBLIC ${wolfssl})
endif()

if(NOT ${IDF_TARGET} STREQUAL "linux")
# Increase link multiplicity to get some lwip symbols correctly resolved by the linker
# due to cyclic dependencies present in IDF for lwip/esp_netif/mbedtls
idf_component_get_property(lwip lwip COMPONENT_LIB)
set_property(TARGET ${lwip} APPEND PROPERTY LINK_INTERFACE_MULTIPLICITY 5)
else()
    # Check if LWIP in the build for linux target to adapt esp-tls compatibility layer
    idf_build_get_property(build_components BUILD_COMPONENTS)
    if(CONFIG_LWIP_ENABLE)
        target_compile_definitions(${COMPONENT_LIB} PRIVATE ESP_TLS_WITH_LWIP=1)
    endif()
endif()


if(CONFIG_ESP_TLS_USE_SECURE_ELEMENT)
    idf_component_optional_requires(PRIVATE espressif__esp-cryptoauthlib esp-cryptoauthlib)
endif()
