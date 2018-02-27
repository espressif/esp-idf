set(COMPONENT_ADD_INCLUDEDIRS
    include/lwip
    include/lwip/port
    include/lwip/posix
    apps/ping
    )

if(CONFIG_PPP_SUPPORT)
    set(LWIP_PPP_DIRS netif/ppp/polarssl netif/ppp)
endif()

set(COMPONENT_SRCDIRS
    api
    apps apps/sntp apps/ping
    core core/ipv4 core/ipv6
    ${LWIP_PPP_DIRS} netif
    port/freertos port/netif port/debug port)

register_component()

component_compile_options(-Wno-address)

# patch around warnings in third-party files
set_source_files_properties(api/tcpip.c
    PROPERTIES COMPILE_FLAGS
    -Wno-unused-variable
    )
set_source_files_properties(core/pbuf.c core/tcp_in.c
    PROPERTIES COMPILE_FLAGS
    -Wno-unused-but-set-variable
    )
set_source_files_properties(apps/dhcpserver.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-unused-variable -Wno-unused-but-set-variable -Wno-type-limits"
    )
