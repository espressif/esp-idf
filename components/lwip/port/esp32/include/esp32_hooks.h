#ifndef __ESP32_HOOKS_H__
#define __ESP32_HOOKS_H__
#include "lwip/ip_addr.h"
#include "lwip/arch.h"
#include "lwip/err.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_LWIP_HOOK_TCP_ISN_MINIX
void lwip_init_tcp_isn(u32_t boot_time, const u8_t *secret_16_bytes);
#endif
#if defined(CONFIG_LWIP_HOOK_TCP_ISN_WEAK) || defined(CONFIG_LWIP_HOOK_TCP_ISN_STRONG) || defined(CONFIG_LWIP_HOOK_TCP_ISN_MINIX)
u32_t lwip_hook_tcp_isn(const ip_addr_t *local_ip, u16_t local_port,
                        const ip_addr_t *remote_ip, u16_t remote_port);
#define LWIP_HOOK_TCP_ISN lwip_hook_tcp_isn
#endif

#if defined(CONFIG_LWIP_HOOK_IP6_ROUTE_WEAK) || defined(CONFIG_LWIP_HOOK_IP6_ROUTE_STRONG)
struct netif *
lwip_hook_ip6_route(const ip6_addr_t *src, const ip6_addr_t *dest);

#define LWIP_HOOK_IP6_ROUTE lwip_hook_ip6_route
#endif

#if defined(CONFIG_LWIP_HOOK_NETCONN_EXTERNAL_RESOLVE_WEAK) || defined(CONFIG_LWIP_HOOK_NETCONN_EXTERNAL_RESOLVE_STRONG)
int lwip_hook_netconn_external_resolve(const char *name, ip_addr_t *addr, u8_t addrtype, err_t *err);

#define LWIP_HOOK_NETCONN_EXTERNAL_RESOLVE lwip_hook_netconn_external_resolve
#endif

#ifdef __cplusplus
}
#endif

#endif
