set(srcs
    "transport.c"
    "transport_ssl.c"
    "transport_internal.c")

if(CONFIG_WS_TRANSPORT)
list(APPEND srcs
    "transport_ws.c")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS "private_include"
                    REQUIRES esp-tls lwip)
