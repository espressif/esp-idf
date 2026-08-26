/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_netif.h"
#include "esp_netif_lwip_internal.h"
#include "lwip/esp_netif_net_stack.h"
#if defined(CONFIG_PPP_SUPPORT)
#include "esp_netif_lwip_ppp.h"
#endif
#if defined(CONFIG_ESP_NETIF_TCPIP_LWIP)

#if CONFIG_ESP_NETIF_BRIDGE_EN
#include "netif/bridgeif.h"

static const struct esp_netif_netstack_config s_br_netif_config = {
        .lwip = {
            .init_fn = bridgeif_init,
            .input_fn = NULL
        }
};
const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_br       = &s_br_netif_config;
#endif // CONFIG_ESP_NETIF_BRIDGE_EN

//
// Purpose of this object is to define default network stack configuration
//  of basic types of interfaces using lwip network stack
//

static const struct esp_netif_netstack_config s_eth_netif_config = {
        .lwip = {
            .init_fn = ethernetif_init,
            .input_fn = ethernetif_input
        }
};
static const struct esp_netif_netstack_config s_wifi_netif_config_ap = {
        .lwip = {
            .init_fn = wlanif_init_ap,
            .input_fn = wlanif_input
        }

};
static const struct esp_netif_netstack_config s_wifi_netif_config_sta = {
        .lwip = {
                .init_fn = wlanif_init_sta,
                .input_fn = wlanif_input
        }
};
static const struct esp_netif_netstack_config s_wifi_netif_config_nan = {
        .lwip = {
                .init_fn = wlanif_init_nan,
                .input_fn = wlanif_input
        }
};

#if defined(CONFIG_PPP_SUPPORT)
static const struct esp_netif_netstack_config s_netif_config_ppp = {
        .lwip_ppp = {
                .input_fn = esp_netif_lwip_ppp_input,
                .ppp_events = {
                        .ppp_error_event_enabled = true,
                        .ppp_phase_event_enabled = false
                }
        }
};
const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_ppp      = &s_netif_config_ppp;
#endif // CONFIG_PPP_SUPPORT

const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_eth      = &s_eth_netif_config;
const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_wifi_sta = &s_wifi_netif_config_sta;
const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_wifi_nan = &s_wifi_netif_config_nan;
const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_wifi_ap  = &s_wifi_netif_config_ap;

#endif /*CONFIG_ESP_NETIF_TCPIP_LWIP*/
