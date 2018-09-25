set(COMPONENT_SRCS "transport.c"
                   "transport_ssl.c"
                   "transport_tcp.c"
                   "transport_ws.c"
                   "transport_utils.c")

set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_REQUIRES lwip esp-tls)

register_component()

if(GCC_NOT_5_2_0)
    # Temporary suppress "format-overflow" warning until it is fixed
    set_source_files_properties(
        transport_ws.c
        PROPERTIES COMPILE_FLAGS
        -Wno-format-overflow)
endif()
