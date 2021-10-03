if(NOT CONFIG_WS_TRANSPORT AND NOT CMAKE_BUILD_EARLY_EXPANSION)
    message(STATUS "Websocket transport is disabled so the esp_websocket_client component will not be built")
    # note: the component is still included in the build so it can become visible again in config
    # without needing to re-run CMake. However no source or header files are built.
    idf_component_register()
    return()
endif()

idf_component_register(SRCS "esp_websocket_client.c"
                    INCLUDE_DIRS "include"
                    REQUIRES lwip esp-tls tcp_transport nghttp
                    PRIV_REQUIRES esp_timer)
