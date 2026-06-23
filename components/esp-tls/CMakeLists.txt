set(srcs esp_tls.c esp-tls-crypto/esp_tls_crypto.c esp_tls_error_capture.c esp_tls_platform_port.c)
if(CONFIG_ESP_TLS_USING_MBEDTLS)
    list(APPEND srcs
        "esp_tls_mbedtls.c")
endif()

if(CONFIG_ESP_TLS_USING_WOLFSSL)
    message(STATUS "esp-tls configured for wolfssl")
    list(APPEND srcs
        "esp_tls_wolfssl.c")
    set(wolfssl_esp_tls_lib "wolfssl")
else()
    unset(wolfssl_esp_tls_lib)
endif()

set(priv_req http_parser esp_timer)
if(NOT ${IDF_TARGET} STREQUAL "linux")
    list(APPEND priv_req lwip)
endif()

message(STATUS "idf_component_register wolfssl_esp_tls_lib: ${wolfssl_esp_tls_lib}")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} esp-tls-crypto
                    PRIV_INCLUDE_DIRS "private_include"
                    # mbedtls is public requirements because esp_tls.h
                    # includes mbedtls header files.
                    REQUIRES mbedtls ${wolfssl_esp_tls_lib}
                    PRIV_REQUIRES ${priv_req})

# When using wolfSSL for the ESP-TLS (see menuconfig),
# There are two options:
#   1) A specified source directory, typically a wolfssl git clone
#   2) The esp-wolfssl
# TODO this is duplicate code. See components/wap_supplicant
message(STATUS "esp-tls config begin")
if(CONFIG_ESP_TLS_USING_WOLFSSL)
    message(STATUS "found CONFIG_ESP_TLS_USING_WOLFSSL")
    # See https://github.com/wolfSSL/wolfssl/
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWOLFSSL_ESPIDF")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWOLFSSL_USER_SETTINGS")

    # The published wolfSSL 5.7.0 user_settings.h does not include some features that
    # might be enabled in Kconfig, so enable them here:
    # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_ALPN")
    # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_SNI")
    # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DOPENSSL_EXTRA_X509_SMALL")
    # this only works for VisualGDB, not idf.py from command-line

    message(STATUS "CMAKE_HOME_DIRECTORY = ${CMAKE_HOME_DIRECTORY}")
    message(STATUS "CMAKE_PARENT_LIST_FILE = ${CMAKE_PARENT_LIST_FILE}")
    message(STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
    message(STATUS "COMPONENT_DIR = ${CMAKE_HOME_DIRECTORY}")
    message(STATUS "COMPONENT_LIB = ${COMPONENT_LIB}")
    message(STATUS "FOUND_WOLFSSL = ${FOUND_WOLFSSL}")
    message(STATUS "PROJECT_DIR = ${PROJECT_DIR}")
    message(STATUS "WOLFSSL_PROJECT_DIR = ${WOLFSSL_PROJECT_DIR}")
    message(STATUS "CMAKE_HOME_DIRECTORY = ${CMAKE_HOME_DIRECTORY}")
    message(STATUS "WOLFSSL_ROOT = ${WOLFSSL_ROOT}")

    if(CONFIG_ESP_TLS_USING_WOLFSSL_SPECIFIED)
        get_filename_component(CUSTOM_SETTING_WOLFSSL_ROOT_PATH "${CUSTOM_SETTING_WOLFSSL_ROOT}" ABSOLUTE)
        if(EXISTS "${CUSTOM_SETTING_WOLFSSL_ROOT_PATH}/wolfcrypt/src")
            message(STATUS "ESP-TLS using wolfSSL in: ${CUSTOM_SETTING_WOLFSSL_ROOT_PATH}")
        else()
            message(STATUS "ESP-TLS specified directory does not contain wolfSSL: ${CUSTOM_SETTING_WOLFSSL_ROOT_PATH}")
        endif()
        idf_component_get_property(wolfssl wolfssl COMPONENT_LIB)
        target_link_libraries(${COMPONENT_LIB} PUBLIC ${wolfssl})
    else()
        # Is wolfSSL installed in the local project as a Managed Component?
        set(WOLFSSL_COMPONENT_SEARCH "${PROJECT_DIR}/managed_components/wolfssl__wolfssl")
        message(STATUS "Searching for wolfSSL in ${WOLFSSL_COMPONENT_SEARCH}")
        if(EXISTS "${WOLFSSL_COMPONENT_SEARCH}")
            message(STATUS "Configuring ESP-IDF to use wolfssl in Managed Component: ${WOLFSSL_COMPONENT_SEARCH}")
            idf_component_get_property(wolfssl wolfssl__wolfssl COMPONENT_LIB)
        else()
            # Is wolfSSL installed in the local project as a Managed Component
            # converted to regular project component?
            set(WOLFSSL_COMPONENT_SEARCH "${PROJECT_DIR}/components/wolfssl__wolfssl")
            message(STATUS "Searching for wolfSSL in ${WOLFSSL_COMPONENT_SEARCH}")
            if(EXISTS "${WOLFSSL_COMPONENT_SEARCH}")
                message(STATUS
                      "Configuring ESP-IDF to use wolfssl in Converted Managed Component: ${WOLFSSL_COMPONENT_SEARCH}")
                idf_component_get_property(wolfssl wolfssl__wolfssl COMPONENT_LIB)
            else()
                # Is wolfSSL installed in the local project as a non-maged, regular component?
                set(WOLFSSL_COMPONENT_SEARCH "${PROJECT_DIR}/components/wolfssl")
                message(STATUS "Searching for wolfSSL in ${WOLFSSL_COMPONENT_SEARCH}")
                if(EXISTS "${WOLFSSL_COMPONENT_SEARCH}")
                    message(STATUS "Configuring ESP-IDF to use wolfssl in Component: ${WOLFSSL_COMPONENT_SEARCH}")
                    idf_component_get_property(wolfssl wolfssl COMPONENT_LIB)
                else()
                    set(WOLFSSL_COMPONENT_SEARCH "${THIS_IDF_PATH}/components/esp-wolfssl")
                    message(STATUS "Searching for wolfSSL in ${WOLFSSL_COMPONENT_SEARCH}")
                    if(EXISTS "${WOLFSSL_COMPONENT_SEARCH}")
                        message(STATUS "Configuring ESP-IDF to use wolfssl from: ${WOLFSSL_COMPONENT_SEARCH}")
                        message(STATUS "Warning: Using legacy esp-wolfssl. Consider using a Managed Component")
                        # See https://github.com/espressif/esp-idf
                        message(STATUS "Configuring ESP-TLS to use esp-wolfssl")
                        idf_component_get_property(wolfssl esp-wolfssl COMPONENT_LIB)
                    else()
                        message(STATUS "Consider installing wolfSSL from "
                                       "https://components.espressif.com/components/wolfssl/wolfssl")
                        message(FATAL_ERROR "Component ${component} not found")
                    endif() # esp-wolfssl
                endif() # project wolfssl
            endif() # project converted wolfssl__wolfssl
        endif() # project managed component wolfssl__wolfssl
        # idf_component_get_property(wolfssl wolfssl__wolfssl COMPONENT_LIB)
        target_link_libraries(${COMPONENT_LIB} PUBLIC ${wolfssl})
    endif()
else()
    message(STATUS "ESP-TLS is not configured to use wolfSSL.")
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
