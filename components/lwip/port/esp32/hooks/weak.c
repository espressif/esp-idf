#include "esp32_hooks.h"

#define __weak __attribute__((weak))

#ifdef CONFIG_LWIP_HOOK_TCP_ISN_WEAK
u32_t __weak lwip_hook_tcp_isn(const ip_addr_t *local_ip, u16_t local_port, const ip_addr_t *remote_ip, u16_t remote_port)
{
    LWIP_UNUSED_ARG(local_ip);
    LWIP_UNUSED_ARG(local_port);
    LWIP_UNUSED_ARG(remote_ip);
    LWIP_UNUSED_ARG(remote_port);

    return 0;
}
#endif

#ifdef CONFIG_LWIP_HOOK_IP6_ROUTE_WEAK
struct netif *__weak
lwip_hook_ip6_route(const ip6_addr_t *src, const ip6_addr_t *dest)
{
    LWIP_UNUSED_ARG(src);
    LWIP_UNUSED_ARG(dest);

    return NULL;
}
#endif

#ifdef CONFIG_LWIP_HOOK_NETCONN_EXTERNAL_RESOLVE_WEAK
int __weak lwip_hook_netconn_external_resolve(const char *name, ip_addr_t *addr, u8_t addrtype, err_t *err)
{
    LWIP_UNUSED_ARG(name);
    LWIP_UNUSED_ARG(addr);
    LWIP_UNUSED_ARG(addrtype);
    LWIP_UNUSED_ARG(err);

    return 0;
}
#endif
