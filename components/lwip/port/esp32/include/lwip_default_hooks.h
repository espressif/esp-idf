// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _LWIP_DEFAULT_HOOKS_H_
#define _LWIP_DEFAULT_HOOKS_H_
#include "lwip/ip_addr.h"
#include "lwip/arch.h"
#include "lwip/err.h"

#ifdef ESP_IDF_LWIP_HOOK_FILENAME
#include ESP_IDF_LWIP_HOOK_FILENAME
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_LWIP_HOOK_TCP_ISN_DEFAULT
void lwip_init_tcp_isn(u32_t boot_time, const u8_t *secret_16_bytes);
#endif
#if defined(CONFIG_LWIP_HOOK_TCP_ISN_CUSTOM) || defined(CONFIG_LWIP_HOOK_TCP_ISN_DEFAULT)
u32_t lwip_hook_tcp_isn(const ip_addr_t *local_ip, u16_t local_port,
                        const ip_addr_t *remote_ip, u16_t remote_port);
#define LWIP_HOOK_TCP_ISN lwip_hook_tcp_isn
#endif /* CONFIG_LWIP_HOOK_TCP_ISN... */

#if defined(CONFIG_LWIP_HOOK_IP6_ROUTE_CUSTOM) || defined(CONFIG_LWIP_HOOK_IP6_ROUTE_DEFAULT)
struct netif *
lwip_hook_ip6_route(const ip6_addr_t *src, const ip6_addr_t *dest);

#define LWIP_HOOK_IP6_ROUTE lwip_hook_ip6_route
#endif /* CONFIG_LWIP_HOOK_IP6_ROUTE... */

#if defined(CONFIG_LWIP_HOOK_NETCONN_EXT_RESOLVE_CUSTOM) || defined(CONFIG_LWIP_HOOK_NETCONN_EXT_RESOLVE_DEFAULT)
int lwip_hook_netconn_external_resolve(const char *name, ip_addr_t *addr, u8_t addrtype, err_t *err);

#define LWIP_HOOK_NETCONN_EXTERNAL_RESOLVE lwip_hook_netconn_external_resolve
#endif /* CONFIG_LWIP_HOOK_NETCONN_EXTERNAL_RESOLVE... */

#ifdef __cplusplus
}
#endif

#endif /* _LWIP_DEFAULT_HOOKS_H_ */
