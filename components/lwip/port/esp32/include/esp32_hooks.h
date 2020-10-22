#ifndef __ESP32_HOOKS_H__
#define __ESP32_HOOKS_H__
#include "lwip/netif.h"
#include "lwip/ip6_addr.h"

#define __weak __attribute__((weak))

struct netif *
lwip_hook_ip6_route(const ip6_addr_t *src, const ip6_addr_t *dest);

#define LWIP_HOOK_IP6_ROUTE lwip_hook_ip6_route

int lwip_hook_netconn_external_resolve(const char *name, ip_addr_t *addr, u8_t addrtype, err_t *err);

#define LWIP_HOOK_NETCONN_EXTERNAL_RESOLVE lwip_hook_netconn_external_resolve

#endif
