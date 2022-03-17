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

#ifdef LWIP_HOOK_IP4_ROUTE_SRC
#if ESP_IP4_ROUTE
#include "lwip/netif.h"

bool ip4_netif_exist(const ip4_addr_t *src, const ip4_addr_t *dest)
{
    struct netif *netif = NULL;

    for (netif = netif_list; netif != NULL; netif = netif->next) {
        /* is the netif up, does it have a link and a valid address? */
        if (netif_is_up(netif) && netif_is_link_up(netif) && !ip4_addr_isany_val(*netif_ip4_addr(netif))) {
            /* source netif and dest netif match? */
            if (ip4_addr_netcmp(src, netif_ip4_addr(netif), netif_ip4_netmask(netif)) || ip4_addr_netcmp(dest, netif_ip4_addr(netif), netif_ip4_netmask(netif))) {
                /* return false when both netif don't match */
                return true;
            }
        }
    }

    return false;
}

/**
 * Source based IPv4 routing hook function.
 */
struct netif *
ip4_route_src_hook(const ip4_addr_t *src,const ip4_addr_t *dest)
{
    struct netif *netif = NULL;

    /* destination IP is broadcast IP? */
    if ((src != NULL) && !ip4_addr_isany(src)) {
        /* iterate through netifs */
        for (netif = netif_list; netif != NULL; netif = netif->next) {
            /* is the netif up, does it have a link and a valid address? */
            if (netif_is_up(netif) && netif_is_link_up(netif) && !ip4_addr_isany_val(*netif_ip4_addr(netif))) {
                /* source IP matches? */
                if (ip4_addr_cmp(src, netif_ip4_addr(netif))) {
                    /* return netif on which to forward IP packet */

                    return netif;
                }
            }
        }
    }
    return netif;
}
#endif
#endif /* LWIP_HOOK_IP4_ROUTE_SRC */
