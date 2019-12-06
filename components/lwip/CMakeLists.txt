set(include_dirs
    include/apps
    include/apps/sntp
    lwip/src/include
    port/esp32/include
    port/esp32/include/arch
    )

set(srcs
    "apps/dhcpserver/dhcpserver.c"
    "apps/ping/esp_ping.c"
    "apps/ping/ping.c"
    "apps/ping/ping_sock.c"
    "apps/sntp/sntp.c"
    "lwip/src/api/api_lib.c"
    "lwip/src/api/api_msg.c"
    "lwip/src/api/err.c"
    "lwip/src/api/if_api.c"
    "lwip/src/api/netbuf.c"
    "lwip/src/api/netdb.c"
    "lwip/src/api/netifapi.c"
    "lwip/src/api/sockets.c"
    "lwip/src/api/tcpip.c"
    "lwip/src/apps/sntp/sntp.c"
    "lwip/src/apps/netbiosns/netbiosns.c"
    "lwip/src/core/def.c"
    "lwip/src/core/dns.c"
    "lwip/src/core/inet_chksum.c"
    "lwip/src/core/init.c"
    "lwip/src/core/ip.c"
    "lwip/src/core/mem.c"
    "lwip/src/core/memp.c"
    "lwip/src/core/netif.c"
    "lwip/src/core/pbuf.c"
    "lwip/src/core/raw.c"
    "lwip/src/core/stats.c"
    "lwip/src/core/sys.c"
    "lwip/src/core/tcp.c"
    "lwip/src/core/tcp_in.c"
    "lwip/src/core/tcp_out.c"
    "lwip/src/core/timeouts.c"
    "lwip/src/core/udp.c"
    "lwip/src/core/ipv4/autoip.c"
    "lwip/src/core/ipv4/dhcp.c"
    "lwip/src/core/ipv4/etharp.c"
    "lwip/src/core/ipv4/icmp.c"
    "lwip/src/core/ipv4/igmp.c"
    "lwip/src/core/ipv4/ip4.c"
    "lwip/src/core/ipv4/ip4_addr.c"
    "lwip/src/core/ipv4/ip4_frag.c"
    "lwip/src/core/ipv6/dhcp6.c"
    "lwip/src/core/ipv6/ethip6.c"
    "lwip/src/core/ipv6/icmp6.c"
    "lwip/src/core/ipv6/inet6.c"
    "lwip/src/core/ipv6/ip6.c"
    "lwip/src/core/ipv6/ip6_addr.c"
    "lwip/src/core/ipv6/ip6_frag.c"
    "lwip/src/core/ipv6/mld6.c"
    "lwip/src/core/ipv6/nd6.c"
    "lwip/src/netif/ethernet.c"
    "lwip/src/netif/lowpan6.c"
    "lwip/src/netif/slipif.c"
    "lwip/src/netif/ppp/auth.c"
    "lwip/src/netif/ppp/ccp.c"
    "lwip/src/netif/ppp/chap-md5.c"
    "lwip/src/netif/ppp/chap-new.c"
    "lwip/src/netif/ppp/chap_ms.c"
    "lwip/src/netif/ppp/demand.c"
    "lwip/src/netif/ppp/eap.c"
    "lwip/src/netif/ppp/ecp.c"
    "lwip/src/netif/ppp/eui64.c"
    "lwip/src/netif/ppp/fsm.c"
    "lwip/src/netif/ppp/ipcp.c"
    "lwip/src/netif/ppp/ipv6cp.c"
    "lwip/src/netif/ppp/lcp.c"
    "lwip/src/netif/ppp/magic.c"
    "lwip/src/netif/ppp/mppe.c"
    "lwip/src/netif/ppp/multilink.c"
    "lwip/src/netif/ppp/ppp.c"
    "lwip/src/netif/ppp/pppapi.c"
    "lwip/src/netif/ppp/pppcrypt.c"
    "lwip/src/netif/ppp/pppoe.c"
    "lwip/src/netif/ppp/pppol2tp.c"
    "lwip/src/netif/ppp/pppos.c"
    "lwip/src/netif/ppp/upap.c"
    "lwip/src/netif/ppp/utils.c"
    "lwip/src/netif/ppp/vj.c"
    "port/esp32/vfs_lwip.c"
    "port/esp32/debug/lwip_debug.c"
    "port/esp32/freertos/sys_arch.c"
    "port/esp32/netif/dhcp_state.c"
    "port/esp32/netif/wlanif.c")

if(CONFIG_LWIP_PPP_SUPPORT)
    list(APPEND srcs
        "lwip/src/netif/ppp/auth.c"
        "lwip/src/netif/ppp/ccp.c"
        "lwip/src/netif/ppp/chap-md5.c"
        "lwip/src/netif/ppp/chap-new.c"
        "lwip/src/netif/ppp/chap_ms.c"
        "lwip/src/netif/ppp/demand.c"
        "lwip/src/netif/ppp/eap.c"
        "lwip/src/netif/ppp/ecp.c"
        "lwip/src/netif/ppp/eui64.c"
        "lwip/src/netif/ppp/fsm.c"
        "lwip/src/netif/ppp/ipcp.c"
        "lwip/src/netif/ppp/ipv6cp.c"
        "lwip/src/netif/ppp/lcp.c"
        "lwip/src/netif/ppp/magic.c"
        "lwip/src/netif/ppp/mppe.c"
        "lwip/src/netif/ppp/multilink.c"
        "lwip/src/netif/ppp/ppp.c"
        "lwip/src/netif/ppp/pppapi.c"
        "lwip/src/netif/ppp/pppcrypt.c"
        "lwip/src/netif/ppp/pppoe.c"
        "lwip/src/netif/ppp/pppol2tp.c"
        "lwip/src/netif/ppp/pppos.c"
        "lwip/src/netif/ppp/upap.c"
        "lwip/src/netif/ppp/utils.c"
        "lwip/src/netif/ppp/vj.c"
        "lwip/src/netif/ppp/polarssl/arc4.c"
        "lwip/src/netif/ppp/polarssl/des.c"
        "lwip/src/netif/ppp/polarssl/md4.c"
        "lwip/src/netif/ppp/polarssl/md5.c"
        "lwip/src/netif/ppp/polarssl/sha1.c")
endif()

if(CONFIG_ETH_ENABLED)
    list(APPEND srcs "port/esp32/netif/ethernetif.c")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}"
                    LDFRAGMENTS linker.lf
                    REQUIRES vfs esp_wifi
                    PRIV_REQUIRES esp_eth esp_netif tcpip_adapter nvs_flash)

# lots of LWIP source files evaluate macros that check address of stack variables
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-address)

if(GCC_NOT_5_2_0)
    set_source_files_properties(
        lwip/src/netif/ppp/ppp.c
        PROPERTIES COMPILE_FLAGS
        -Wno-uninitialized
        )
    set_source_files_properties(
        lwip/src/netif/ppp/pppos.c
        PROPERTIES COMPILE_FLAGS
        -Wno-implicit-fallthrough
        )
endif()
