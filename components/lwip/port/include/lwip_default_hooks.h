/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LWIP_ESP_DEFAULT_HOOKS_H_
#define LWIP_ESP_DEFAULT_HOOKS_H_
#include "lwip/ip_addr.h"
#include "lwip/arch.h"
#include "lwip/err.h"
#include "lwip/pbuf.h"
#include "netif/dhcp_state.h"


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

#if defined(CONFIG_LWIP_HOOK_ND6_GET_GW_CUSTOM) || defined(CONFIG_LWIP_HOOK_ND6_GET_GW_DEFAULT)
const ip6_addr_t *lwip_hook_nd6_get_gw(struct netif *netif, const ip6_addr_t *dest);

#define LWIP_HOOK_ND6_GET_GW lwip_hook_nd6_get_gw
#endif /* CONFIG_LWIP_HOOK_ND6_GET_GATEWAY... */

#if defined(CONFIG_LWIP_HOOK_IP6_SELECT_SRC_ADDR_CUSTOM) || defined(CONFIG_LWIP_HOOK_IP6_SELECT_SRC_ADDR_DEFAULT)
const ip_addr_t *lwip_hook_ip6_select_source_address(struct netif *netif, const ip6_addr_t *dest);

#define LWIP_HOOK_IP6_SELECT_SRC_ADDR lwip_hook_ip6_select_source_address
#endif /* CONFIG_LWIP_HOOK_IP6_SELECT_SRC_ADDR... */

#if defined(CONFIG_LWIP_HOOK_NETCONN_EXT_RESOLVE_CUSTOM) || defined(CONFIG_LWIP_HOOK_NETCONN_EXT_RESOLVE_DEFAULT)
int lwip_hook_netconn_external_resolve(const char *name, ip_addr_t *addr, u8_t addrtype, err_t *err);

#define LWIP_HOOK_NETCONN_EXTERNAL_RESOLVE lwip_hook_netconn_external_resolve
#endif /* CONFIG_LWIP_HOOK_NETCONN_EXTERNAL_RESOLVE... */

#if defined(CONFIG_LWIP_HOOK_IP6_INPUT_CUSTOM) || defined(CONFIG_LWIP_HOOK_IP6_INPUT_DEFAULT)
int lwip_hook_ip6_input(struct pbuf *p, struct netif *inp);

#define LWIP_HOOK_IP6_INPUT lwip_hook_ip6_input
#endif /* CONFIG_LWIP_HOOK_IP6_INPUT_CUSTIOM... */

#ifdef CONFIG_LWIP_IPV4
struct netif *
ip4_route_src_hook(const ip4_addr_t *src,const ip4_addr_t *dest);

struct dhcp;
struct netif;
struct dhcp_msg;
void dhcp_parse_extra_opts(struct dhcp *dhcp, uint8_t state, uint8_t option, uint8_t len, struct pbuf* p, uint16_t offset);

void dhcp_append_extra_opts(struct netif *netif, uint8_t state, struct dhcp_msg *msg_out, uint16_t *options_out_len);

int dhcp_set_vendor_class_identifier(uint8_t len, const char * str);
int dhcp_get_vendor_specific_information(uint8_t len, char * str);
void dhcp_free_vendor_class_identifier(void);
#endif /* CONFIG_LWIP_IPV4 */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_ESP_DEFAULT_HOOKS_H_ */
