/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <lwip/dns.h>
#include <lwip/timeouts.h>
#include <lwip/etharp.h>

#include "esp_check.h"
#include "esp_netif_lwip_internal.h"
#include "esp_netif_lwip_orig.h"

#include "esp_netif.h"
#include "esp_netif_private.h"

#define DHCP_CB_CHANGE (LWIP_NSC_IPV4_SETTINGS_CHANGED | LWIP_NSC_IPV4_ADDRESS_CHANGED | LWIP_NSC_IPV4_GATEWAY_CHANGED | LWIP_NSC_IPV4_NETMASK_CHANGED)

static void netif_callback_fn(struct netif* netif, netif_nsc_reason_t reason, const netif_ext_callback_args_t* args)
{
    if (reason & DHCP_CB_CHANGE) {
        esp_netif_internal_dhcpc_cb(netif);
    }
#if LWIP_IPV6
    if (reason & LWIP_NSC_IPV6_ADDR_STATE_CHANGED) {
        s8_t addr_idx = args->ipv6_addr_state_changed.addr_index;
        if (netif_ip6_addr_state(netif, addr_idx) & IP6_ADDR_VALID)  {
            /* address is valid -> call the callback function */
            esp_netif_internal_nd6_cb(netif, addr_idx);
        }
    }
#endif /* #if LWIP_IPV6 */
}

static netif_ext_callback_t netif_callback = { .callback_fn = NULL, .next = NULL };

void set_lwip_netif_callback(void)
{
    if (netif_callback.callback_fn == NULL ) {
        netif_add_ext_callback(&netif_callback, netif_callback_fn);
    }
}

void remove_lwip_netif_callback(void)
{
    netif_remove_ext_callback(&netif_callback);
    memset(&netif_callback, 0, sizeof(netif_callback));
}

void dns_clear_servers(bool keep_fallback)
{
    u8_t numdns = 0;

    for (numdns = 0; numdns < DNS_MAX_SERVERS; numdns ++) {
        if (keep_fallback && numdns == DNS_FALLBACK_SERVER_INDEX) {
            continue;
        }

        dns_setserver(numdns, NULL);
    }
}

#ifdef CONFIG_LWIP_GARP_TMR_INTERVAL
void netif_send_garp(void *arg)
{
    struct netif *netif = arg;
    etharp_gratuitous(netif);
    sys_timeout(CONFIG_LWIP_GARP_TMR_INTERVAL*1000, netif_send_garp, netif);
}

void netif_set_garp_flag(struct netif *netif)
{
    sys_timeout(CONFIG_LWIP_GARP_TMR_INTERVAL*1000, netif_send_garp, netif);
}

void netif_unset_garp_flag(struct netif *netif)
{
    sys_untimeout(netif_send_garp, netif);
}
#endif  // CONFIG_LWIP_GARP_TMR_INTERVAL
