set(COMPONENT_ADD_INCLUDEDIRS
    include/lwip
    include/lwip/port
    include/lwip/posix
    apps/ping
    )

set(COMPONENT_SRCS "api/api_lib.c"
                   "api/api_msg.c"
                   "api/err.c"
                   "api/netbuf.c"
                   "api/netdb.c"
                   "api/netifapi.c"
                   "api/pppapi.c"
                   "api/sockets.c"
                   "api/tcpip.c"
                   "apps/dhcpserver.c"
                   "apps/ping/esp_ping.c"
                   "apps/ping/ping.c"
                   "apps/sntp/sntp.c"
                   "core/def.c"
                   "core/dns.c"
                   "core/inet_chksum.c"
                   "core/init.c"
                   "core/ip.c"
                   "core/mem.c"
                   "core/memp.c"
                   "core/netif.c"
                   "core/pbuf.c"
                   "core/raw.c"
                   "core/stats.c"
                   "core/sys.c"
                   "core/tcp.c"
                   "core/tcp_in.c"
                   "core/tcp_out.c"
                   "core/timers.c"
                   "core/udp.c"
                   "core/ipv4/autoip.c"
                   "core/ipv4/dhcp.c"
                   "core/ipv4/icmp.c"
                   "core/ipv4/igmp.c"
                   "core/ipv4/ip4.c"
                   "core/ipv4/ip4_addr.c"
                   "core/ipv4/ip_frag.c"
                   "core/ipv6/dhcp6.c"
                   "core/ipv6/ethip6.c"
                   "core/ipv6/icmp6.c"
                   "core/ipv6/inet6.c"
                   "core/ipv6/ip6.c"
                   "core/ipv6/ip6_addr.c"
                   "core/ipv6/ip6_frag.c"
                   "core/ipv6/mld6.c"
                   "core/ipv6/nd6.c"
                   "netif/etharp.c"
                   "netif/ethernet.c"
                   "netif/ethernetif.c"
                   "netif/lowpan6.c"
                   "netif/slipif.c"
                   "port/vfs_lwip.c"
                   "port/debug/lwip_debug.c"
                   "port/freertos/sys_arch.c"
                   "port/netif/ethernetif.c"
                   "port/netif/wlanif.c")

if(CONFIG_PPP_SUPPORT)
    list(APPEND COMPONENT_SRCS "netif/ppp/auth.c"
                   "netif/ppp/ccp.c"
                   "netif/ppp/chap-md5.c"
                   "netif/ppp/chap-new.c"
                   "netif/ppp/chap_ms.c"
                   "netif/ppp/demand.c"
                   "netif/ppp/eap.c"
                   "netif/ppp/ecp.c"
                   "netif/ppp/eui64.c"
                   "netif/ppp/fsm.c"
                   "netif/ppp/ipcp.c"
                   "netif/ppp/ipv6cp.c"
                   "netif/ppp/lcp.c"
                   "netif/ppp/magic.c"
                   "netif/ppp/mppe.c"
                   "netif/ppp/multilink.c"
                   "netif/ppp/ppp.c"
                   "netif/ppp/pppcrypt.c"
                   "netif/ppp/pppoe.c"
                   "netif/ppp/pppol2tp.c"
                   "netif/ppp/pppos.c"
                   "netif/ppp/upap.c"
                   "netif/ppp/utils.c"
                   "netif/ppp/vj.c"
                   "netif/ppp/polarssl/arc4.c"
                   "netif/ppp/polarssl/des.c"
                   "netif/ppp/polarssl/md4.c"
                   "netif/ppp/polarssl/md5.c"
                   "netif/ppp/polarssl/sha1.c")
endif()

set(COMPONENT_REQUIRES vfs)
set(COMPONENT_PRIV_REQUIRES ethernet tcpip_adapter)

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
set_source_files_properties(netif/ppp/pppos.c
    PROPERTIES COMPILE_FLAGS
    -Wno-type-limits)
