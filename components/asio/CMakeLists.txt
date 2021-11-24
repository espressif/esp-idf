if(NOT CONFIG_LWIP_IPV6 AND NOT CMAKE_BUILD_EARLY_EXPANSION)
    # note: the component is still included in the build so it can become visible again in config
    # without needing to re-run CMake. However no source or header files are built.
    message(STATUS "IPV6 support is disabled so the asio component will not be built")
    idf_component_register()
    return()
endif()

set(asio_sources "asio/asio/src/asio.cpp")

if(CONFIG_ASIO_SSL_SUPPORT)
    if(CONFIG_ASIO_USE_ESP_OPENSSL)
        list(APPEND asio_sources
                "asio/asio/src/asio_ssl.cpp"
                "port/src/esp_asio_openssl_stubs.c")
    endif()

    if(CONFIG_ASIO_USE_ESP_WOLFSSL)
        list(APPEND asio_sources
                "asio/asio/src/asio_ssl.cpp")
    endif()
endif()

idf_component_register(SRCS ${asio_sources}
                    INCLUDE_DIRS "asio/asio/include" "port/include"
                    REQUIRES lwip)

if(CONFIG_ASIO_SSL_SUPPORT)
    if(CONFIG_ASIO_USE_ESP_WOLFSSL)
        idf_component_get_property(wolflib esp-wolfssl COMPONENT_LIB)
        idf_component_get_property(wolfdir esp-wolfssl COMPONENT_DIR)

        target_link_libraries(${COMPONENT_LIB} PUBLIC ${wolflib})
        target_include_directories(${COMPONENT_LIB} PUBLIC ${wolfdir}/wolfssl/wolfssl)
    endif()

    if(CONFIG_ASIO_USE_ESP_OPENSSL)
        idf_component_get_property(esp_openssl openssl COMPONENT_LIB)
        target_link_libraries(${COMPONENT_LIB} PUBLIC ${esp_openssl})
    endif()
endif()
