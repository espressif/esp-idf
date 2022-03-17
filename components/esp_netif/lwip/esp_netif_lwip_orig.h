/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_netif_lwip_internal.h"


#if defined(CONFIG_ESP_NETIF_TCPIP_LWIP_ORIG) || defined(CONFIG_ESP_NETIF_TCPIP_LWIP)

#if PPP_SUPPORT
typedef struct ppp_pcb_s ppp_pcb;
#endif

/**
 * @brief Sets one extended lwip netif callbacks for all esp-netif
 */
void set_lwip_netif_callback(void);

void remove_lwip_netif_callback(void);

void esp_netif_internal_dhcpc_cb(struct netif *netif);

void esp_netif_internal_nd6_cb(struct netif *netif, uint8_t index);

static inline void dhcp_set_cb(struct netif *netif, void (*cb)(struct netif*)) { }

static inline void nd6_set_cb(struct netif *netif, void (*cb)(struct netif *netif, u8_t ip_index)) { }

void dns_clear_servers(bool keep_fallback);

#if PPP_SUPPORT && PPP_AUTH_SUPPORT
void pppapi_set_auth(ppp_pcb *pcb, u8_t authtype, const char *user, const char *passwd);
#endif

#if ESP_GRATUITOUS_ARP

void netif_set_garp_flag(struct netif *netif);

void netif_unset_garp_flag(struct netif *netif);

#endif // ESP_GRATUITOUS_ARP

#else // !CONFIG_ESP_NETIF_TCPIP_LWIP_ORIG and !CONFIG_ESP_NETIF_TCPIP_LWIP

static inline void set_lwip_netif_callback(void) { }

static inline void remove_lwip_netif_callback(void) { }

static inline void netif_unset_garp_flag(struct netif *netif) {}

#endif // CONFIG_ESP_NETIF_TCPIP_LWIP_ORIG or CONFIG_ESP_NETIF_TCPIP_LWIP
