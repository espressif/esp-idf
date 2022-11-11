set(srcs esp_tls.c esp-tls-crypto/esp_tls_crypto.c esp_tls_error_capture.c)
if(CONFIG_ESP_TLS_USING_MBEDTLS)
    list(APPEND srcs
        "esp_tls_mbedtls.c")
endif()

if(CONFIG_ESP_TLS_USING_WOLFSSL)
    list(APPEND srcs
        "esp_tls_wolfssl.c")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS . esp-tls-crypto
                    PRIV_INCLUDE_DIRS "private_include"
                    # mbedtls is public requirements becasue esp_tls.h
                    # includes mbedtls header files.
                    REQUIRES mbedtls
                    PRIV_REQUIRES lwip http_parser)

if(CONFIG_ESP_TLS_USING_WOLFSSL)
    idf_component_get_property(wolfssl esp-wolfssl COMPONENT_LIB)
    target_link_libraries(${COMPONENT_LIB} PUBLIC ${wolfssl})
endif()

# Increase link multiplicity to get some lwip symbols correctly resolved by the linker
# due to cyclic dependencies present in IDF for lwip/esp_netif/mbedtls
idf_component_get_property(lwip lwip COMPONENT_LIB)
set_property(TARGET ${lwip} APPEND PROPERTY LINK_INTERFACE_MULTIPLICITY 5)

if(CONFIG_ESP_TLS_USE_SECURE_ELEMENT)
    idf_component_optional_requires(PRIVATE espressif__esp-cryptoauthlib esp-cryptoauthlib)
endif()
