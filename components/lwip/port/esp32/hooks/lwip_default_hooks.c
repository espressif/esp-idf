/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "lwip_default_hooks.h"

#define __weak __attribute__((weak))

#ifdef CONFIG_LWIP_HOOK_IP6_ROUTE_DEFAULT
struct netif *__weak
lwip_hook_ip6_route(const ip6_addr_t *src, const ip6_addr_t *dest)
{
    LWIP_UNUSED_ARG(src);
    LWIP_UNUSED_ARG(dest);

    return NULL;
}
#endif

#ifdef CONFIG_LWIP_HOOK_NETCONN_EXT_RESOLVE_DEFAULT
int __weak lwip_hook_netconn_external_resolve(const char *name, ip_addr_t *addr, u8_t addrtype, err_t *err)
{
    LWIP_UNUSED_ARG(name);
    LWIP_UNUSED_ARG(addr);
    LWIP_UNUSED_ARG(addrtype);
    LWIP_UNUSED_ARG(err);

    return 0;
}
#endif

#ifdef CONFIG_LWIP_HOOK_ND6_GET_GW_DEFAULT
const ip6_addr_t *__weak lwip_hook_nd6_get_gw(struct netif *netif, const ip6_addr_t *dest)
{
    LWIP_UNUSED_ARG(netif);
    LWIP_UNUSED_ARG(dest);

    return 0;
}
#endif

#ifdef CONFIG_LWIP_HOOK_IP6_INPUT_DEFAULT
int __weak lwip_hook_ip6_input(struct pbuf *p, struct netif *inp)
{
    LWIP_UNUSED_ARG(p);
    LWIP_UNUSED_ARG(inp);

    return 0;
}
#endif
