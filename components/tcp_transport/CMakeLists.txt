set(srcs
    "transport.c"
    "transport_ssl.c"
    "transport_internal.c")

if(CONFIG_LWIP_IPV4)
list(APPEND srcs
    "transport_socks_proxy.c")
endif()

if(CONFIG_WS_TRANSPORT)
list(APPEND srcs
    "transport_ws.c")
endif()

set(req esp-tls)
if(NOT ${IDF_TARGET} STREQUAL "linux")
    list(APPEND req lwip esp_timer)
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS "private_include"
                    REQUIRES ${req})

if(${IDF_TARGET} STREQUAL "linux")
    # Check if LWIP in the build for linux target to add esp_timer to the dependencies
    # since socks_proxy transport needs it and lwip & linux build could use it
    idf_build_get_property(build_components BUILD_COMPONENTS)
    if("lwip" IN_LIST build_components)
        idf_component_get_property(esp_timer esp_timer COMPONENT_LIB)
        target_link_libraries(${COMPONENT_LIB} PUBLIC ${esp_timer})
    endif()
endif()
