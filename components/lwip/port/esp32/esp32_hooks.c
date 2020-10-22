#include "esp32_hooks.h"
#include "lwip/arch.h"

struct netif *__weak
lwip_hook_ip6_route(const ip6_addr_t *src, const ip6_addr_t *dest)
{
    LWIP_UNUSED_ARG(src);
    LWIP_UNUSED_ARG(dest);

    return NULL;
}

int __weak lwip_hook_netconn_external_resolve(const char *name, ip_addr_t *addr, u8_t addrtype, err_t *err)
{
    LWIP_UNUSED_ARG(name);
    LWIP_UNUSED_ARG(addr);
    LWIP_UNUSED_ARG(addrtype);
    LWIP_UNUSED_ARG(err);

    return 0;
}
