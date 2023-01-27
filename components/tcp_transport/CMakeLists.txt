set(srcs
    "transport.c"
    "transport_ssl.c"
    "transport_internal.c")

if(CONFIG_WS_TRANSPORT)
list(APPEND srcs
    "transport_ws.c")
endif()

set(req esp-tls)
if(NOT ${IDF_TARGET} STREQUAL "linux")
    list(APPEND req lwip)
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS "private_include"
                    REQUIRES ${req})
