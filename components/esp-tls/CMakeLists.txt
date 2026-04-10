set(srcs esp_tls.c esp-tls-crypto/esp_tls_crypto.c esp_tls_error_capture.c esp_tls_platform_port.c)
if(CONFIG_ESP_TLS_USING_MBEDTLS)
    list(APPEND srcs
        "esp_tls_mbedtls.c")
endif()

if(CONFIG_ESP_TLS_CUSTOM_STACK)
    list(APPEND srcs
        "esp_tls_custom_stack.c")
endif()

set(priv_req http_parser esp_timer)
if(NOT ${IDF_TARGET} STREQUAL "linux")
    list(APPEND priv_req lwip)
else()
    # For linux target on Build system v2, add lwip to PRIV_REQUIRES when CONFIG_LWIP_ENABLE is set.
    if(CONFIG_LWIP_ENABLE AND IDF_BUILD_V2)
        list(APPEND priv_req lwip)
    endif()
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} esp-tls-crypto
                    PRIV_INCLUDE_DIRS "private_include"
                    REQUIRES mbedtls
                    PRIV_REQUIRES ${priv_req})


if(NOT ${IDF_TARGET} STREQUAL "linux")
# Increase link multiplicity to get some lwip symbols correctly resolved by the linker
# due to cyclic dependencies present in IDF for lwip/esp_netif/mbedtls
idf_component_get_property(lwip lwip COMPONENT_LIB)
set_property(TARGET ${lwip} APPEND PROPERTY LINK_INTERFACE_MULTIPLICITY 5)
else()
    # For linux target, define ESP_TLS_WITH_LWIP if LWIP is enabled in the build
    if(CONFIG_LWIP_ENABLE)
        target_compile_definitions(${COMPONENT_LIB} PRIVATE ESP_TLS_WITH_LWIP=1)
    endif()
endif()


if(CONFIG_ESP_TLS_USE_SECURE_ELEMENT)
    idf_component_optional_requires(PRIVATE espressif__esp-cryptoauthlib esp-cryptoauthlib)
endif()
