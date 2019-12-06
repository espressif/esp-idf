// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_netif.h"
#include "esp_private/wifi.h"
#include "esp_log.h"
#include "esp_netif_net_stack.h"

#if CONFIG_ESP_NETIF_TCPIP_ADAPTER_COMPATIBLE_LAYER

#if CONFIG_ETH_ENABLED
#include "esp_eth.h"
#endif
#include "tcpip_adapter_types.h"
#include "esp_wifi_default.h"

//
// Accessing some internal default interfaces and esp-netifs
//
extern void _esp_wifi_set_default_ap_netif(esp_netif_t* esp_netif);
extern void _esp_wifi_set_default_sta_netif(esp_netif_t* esp_netif);
extern esp_err_t _esp_wifi_set_default_wifi_handlers(void);
extern esp_err_t _esp_wifi_clear_default_wifi_handlers(void);
extern esp_err_t esp_netif_up(esp_netif_t *esp_netif);
extern esp_err_t esp_netif_down(esp_netif_t *esp_netif);

//
// Purpose of this module is to provide backward compatible version of esp-netif
// with legacy tcpip_adapter interface
//

static const char* TAG = "tcpip_adapter_compat";

static esp_netif_t *s_esp_netifs[TCPIP_ADAPTER_IF_MAX] = { NULL };
static const char* s_netif_keyif[TCPIP_ADAPTER_IF_MAX] = {
        "WIFI_STA_DEF",
        "WIFI_AP_DEF",
        "ETH_DEF",
};

static bool s_tcpip_adapter_compat = false;

static void wifi_create_and_start_ap(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{
    if (s_esp_netifs[TCPIP_ADAPTER_IF_AP] == NULL) {
        esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_AP();
        esp_netif_t *ap_netif = esp_netif_new(&cfg);

        esp_netif_attach_wifi_ap(ap_netif);
        esp_wifi_set_default_wifi_sta_handlers();
        s_esp_netifs[TCPIP_ADAPTER_IF_AP] = ap_netif;
    }
}

static void wifi_create_and_start_sta(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{
    if (s_esp_netifs[TCPIP_ADAPTER_IF_STA] == NULL) {
        esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_STA();
        esp_netif_t *sta_netif = esp_netif_new(&cfg);

        esp_netif_attach_wifi_station(sta_netif);
        esp_wifi_set_default_wifi_sta_handlers();
        s_esp_netifs[TCPIP_ADAPTER_IF_STA] = sta_netif;
    }
}

static inline esp_netif_t * netif_from_if(tcpip_adapter_if_t interface)
{
    if (interface < TCPIP_ADAPTER_IF_MAX) {
        if (s_esp_netifs[interface] == NULL) {
            s_esp_netifs[interface] = esp_netif_get_handle_from_ifkey(s_netif_keyif[interface]);
            if (s_esp_netifs[interface] == NULL && s_tcpip_adapter_compat) {
                // if not found in compat mode -> create it
                if (interface == TCPIP_ADAPTER_IF_STA) {
                    wifi_create_and_start_sta(NULL, 0, 0, NULL);
                    s_esp_netifs[interface] = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
                } else if (interface == TCPIP_ADAPTER_IF_AP) {
                    wifi_create_and_start_ap(NULL, 0, 0, NULL);
                    s_esp_netifs[interface] = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
                }
            }
        }
        return s_esp_netifs[interface];
    }
    return NULL;
}

void tcpip_adapter_init(void)
{
    s_tcpip_adapter_compat = true;
    esp_err_t err;
    if (ESP_OK != (err = esp_netif_init())) {
        ESP_LOGE(TAG, "ESP-NETIF initialization failed with %d in tcpip_adapter compatibility mode", err);
    }
}

#if CONFIG_ETH_ENABLED
static void tcpip_adapter_attach_eth_to_netif(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t*)data;
    esp_netif_attach(esp_netif, eth_handle);
}

esp_err_t tcpip_adapter_clear_default_eth_handlers(void)
{
    ESP_ERROR_CHECK(esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_START, tcpip_adapter_attach_eth_to_netif));
    return esp_eth_clear_default_handlers(netif_from_if(TCPIP_ADAPTER_IF_ETH));
}

esp_err_t tcpip_adapter_set_default_eth_handlers(void)
{
    if (s_tcpip_adapter_compat) {
        esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
        esp_netif_t *eth_netif = esp_netif_new(&cfg);

        s_esp_netifs[TCPIP_ADAPTER_IF_ETH] = eth_netif;
        // provide a separate "after driver start" hook to attach
        esp_err_t ret = esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_START, tcpip_adapter_attach_eth_to_netif, eth_netif);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to register ");
            return ret;
        }

        return esp_eth_set_default_handlers(eth_netif);
    }
    return ESP_OK;

}
#endif

esp_err_t tcpip_adapter_eth_input(void *buffer, uint16_t len, void *eb)
{
    return esp_netif_receive(netif_from_if(TCPIP_ADAPTER_IF_ETH), buffer, len, eb);
}

esp_err_t tcpip_adapter_sta_input(void *buffer, uint16_t len, void *eb)
{
    return esp_netif_receive(netif_from_if(TCPIP_ADAPTER_IF_STA), buffer, len, eb);
}

esp_err_t tcpip_adapter_ap_input(void *buffer, uint16_t len, void *eb)
{
    return esp_netif_receive(netif_from_if(TCPIP_ADAPTER_IF_AP), buffer, len, eb);
}

esp_err_t tcpip_adapter_compat_start_eth(void* eth_driver)
{
    if (s_tcpip_adapter_compat) {
        esp_netif_t *esp_netif = netif_from_if(TCPIP_ADAPTER_IF_ETH);
        esp_netif_attach(esp_netif, eth_driver);
    }
    return ESP_OK;
}

esp_err_t tcpip_adapter_set_default_wifi_handlers(void)
{
#if CONFIG_ESP_NETIF_TCPIP_ADAPTER_COMPATIBLE_LAYER
    if (s_tcpip_adapter_compat) {
        // create instances and register default handlers only on start event
        esp_err_t err = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_START, wifi_create_and_start_sta, NULL);
        if (err != ESP_OK) {
            return err;
        }
        err = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_START, wifi_create_and_start_ap, NULL);
        if (err != ESP_OK) {
            return err;
        }
        _esp_wifi_set_default_wifi_handlers();
    }
    return ESP_OK;
#else
    ESP_LOGE(TAG, "%s: tcpip adapter compatibility layer is disabled", __func__);
    return ESP_ERR_INVALID_STATE;
#endif
}

esp_err_t tcpip_adapter_clear_default_wifi_handlers(void)
{
    return _esp_wifi_clear_default_wifi_handlers();
}

tcpip_adapter_if_t tcpip_adapter_if_from_esp_netif(esp_netif_t *esp_netif)
{
    for (int i=0; i<TCPIP_ADAPTER_IF_MAX; ++i) {
        if (esp_netif == s_esp_netifs[i])
            return i;
    }
    return TCPIP_ADAPTER_IF_MAX;
}

esp_err_t tcpip_adapter_get_ip_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_info_t *ip_info)
{
    return esp_netif_get_ip_info(netif_from_if(tcpip_if), (esp_netif_ip_info_t *)ip_info);
}

esp_err_t tcpip_adapter_get_ip6_linklocal(tcpip_adapter_if_t tcpip_if, ip6_addr_t *if_ip6)
{
    return esp_netif_get_ip6_linklocal(netif_from_if(tcpip_if), (esp_ip6_addr_t*)if_ip6);
}

esp_err_t tcpip_adapter_dhcpc_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status)
{
    return esp_netif_dhcpc_get_status(netif_from_if(tcpip_if), status);
}

bool tcpip_adapter_is_netif_up(tcpip_adapter_if_t tcpip_if)
{
    return esp_netif_is_netif_up(netif_from_if(tcpip_if));
}

esp_err_t tcpip_adapter_get_netif(tcpip_adapter_if_t tcpip_if, void ** netif)
{
    esp_netif_t *esp_netif = netif_from_if(tcpip_if);
    if (esp_netif) {
        void* net_stack_netif = esp_netif_get_netif_impl(esp_netif);
        *netif = net_stack_netif;
        return ESP_OK;
    }
    return ESP_ERR_INVALID_ARG;
}

esp_err_t tcpip_adapter_create_ip6_linklocal(tcpip_adapter_if_t tcpip_if)
{
    return esp_netif_create_ip6_linklocal(netif_from_if(tcpip_if));
}

esp_err_t tcpip_adapter_dhcps_stop(tcpip_adapter_if_t tcpip_if)
{
    return esp_netif_dhcps_stop(netif_from_if(tcpip_if));
}

esp_err_t tcpip_adapter_dhcpc_stop(tcpip_adapter_if_t tcpip_if)
{
    return esp_netif_dhcpc_stop(netif_from_if(tcpip_if));
}

esp_err_t tcpip_adapter_dhcps_start(tcpip_adapter_if_t tcpip_if)
{
    return esp_netif_dhcps_start(netif_from_if(tcpip_if));
}

esp_err_t tcpip_adapter_dhcpc_start(tcpip_adapter_if_t tcpip_if)
{
    return esp_netif_dhcpc_start(netif_from_if(tcpip_if));
}
esp_err_t tcpip_adapter_dhcps_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status)
{
    return esp_netif_dhcps_get_status(netif_from_if(tcpip_if), status);
}

esp_err_t tcpip_adapter_dhcps_option(tcpip_adapter_dhcp_option_mode_t opt_op, tcpip_adapter_dhcp_option_id_t opt_id, void *opt_val, uint32_t opt_len)
{
    // Note: legacy mode supports dhcps only for default wifi AP
    return esp_netif_dhcps_option(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), opt_op, opt_id, opt_val, opt_len);
}

esp_err_t tcpip_adapter_dhcpc_option(tcpip_adapter_dhcp_option_mode_t opt_op, tcpip_adapter_dhcp_option_id_t opt_id, void *opt_val, uint32_t opt_len)
{
    return esp_netif_dhcpc_option(NULL, opt_op, opt_id, opt_val, opt_len);
}

esp_err_t tcpip_adapter_set_ip_info(tcpip_adapter_if_t tcpip_if, const tcpip_adapter_ip_info_t *ip_info)
{
    return esp_netif_set_ip_info(netif_from_if(tcpip_if), (esp_netif_ip_info_t *)ip_info);
}

esp_err_t tcpip_adapter_get_dns_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dns_type_t type, tcpip_adapter_dns_info_t *dns)
{
    return esp_netif_get_dns_info(netif_from_if(tcpip_if), type, dns);
}

esp_err_t tcpip_adapter_set_dns_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dns_type_t type, tcpip_adapter_dns_info_t *dns)
{
    return esp_netif_set_dns_info(netif_from_if(tcpip_if), type, dns);
}

int tcpip_adapter_get_netif_index(tcpip_adapter_if_t tcpip_if)
{
    return esp_netif_get_netif_impl_index(netif_from_if(tcpip_if));
}

esp_err_t tcpip_adapter_get_sta_list(const wifi_sta_list_t *wifi_sta_list, tcpip_adapter_sta_list_t *tcpip_sta_list)
{
    return esp_netif_get_sta_list(wifi_sta_list, tcpip_sta_list);
}

static esp_err_t tcpip_adapter_compat_start_netif(esp_netif_t *netif, uint8_t *mac, tcpip_adapter_ip_info_t *ip_info)
{
    if (netif == NULL || mac == NULL || ip_info == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }
    esp_netif_set_mac(netif, mac);
    esp_netif_set_ip_info(netif, (esp_netif_ip_info_t *)ip_info);
    esp_netif_action_start(netif, NULL, 0, NULL);
    return ESP_OK;
}

esp_err_t tcpip_adapter_eth_start(uint8_t *mac, tcpip_adapter_ip_info_t *ip_info, void *args)
{
    return tcpip_adapter_compat_start_netif(netif_from_if(TCPIP_ADAPTER_IF_ETH),
                                            mac, ip_info);
}

esp_err_t tcpip_adapter_sta_start(uint8_t *mac, tcpip_adapter_ip_info_t *ip_info)
{
    return tcpip_adapter_compat_start_netif(netif_from_if(TCPIP_ADAPTER_IF_STA),
                                            mac, ip_info);
}

esp_err_t tcpip_adapter_ap_start(uint8_t *mac, tcpip_adapter_ip_info_t *ip_info)
{
    return tcpip_adapter_compat_start_netif(netif_from_if(TCPIP_ADAPTER_IF_AP),
                                            mac, ip_info);
}

esp_err_t tcpip_adapter_stop(tcpip_adapter_if_t tcpip_if)
{
    esp_netif_t *netif = netif_from_if(tcpip_if);
    if (netif == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }
    esp_netif_action_stop(netif_from_if(tcpip_if), NULL, 0, NULL);
    return ESP_OK;
}

esp_err_t tcpip_adapter_up(tcpip_adapter_if_t tcpip_if)
{
    return esp_netif_up(netif_from_if(tcpip_if));
}

esp_err_t tcpip_adapter_down(tcpip_adapter_if_t tcpip_if)
{
    return esp_netif_down(netif_from_if(tcpip_if));
}

esp_err_t tcpip_adapter_get_old_ip_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_info_t *ip_info)
{
    return esp_netif_get_old_ip_info(netif_from_if(tcpip_if), (esp_netif_ip_info_t *)ip_info);
}

esp_err_t tcpip_adapter_set_old_ip_info(tcpip_adapter_if_t tcpip_if, const tcpip_adapter_ip_info_t *ip_info)
{
    return esp_netif_set_old_ip_info(netif_from_if(tcpip_if), (esp_netif_ip_info_t *)ip_info);
}

esp_interface_t tcpip_adapter_get_esp_if(void *dev)
{
    esp_netif_t *netif = esp_netif_get_handle_from_netif_impl(dev);
    for (int i=0; i< TCPIP_ADAPTER_IF_MAX; ++i) {
        if (s_esp_netifs[i] == netif) {
            return i;
        }
    }
    return ESP_IF_MAX;
}

esp_err_t tcpip_adapter_set_hostname(tcpip_adapter_if_t tcpip_if, const char *hostname)
{
    return esp_netif_set_hostname(netif_from_if(tcpip_if), hostname);
}

esp_err_t tcpip_adapter_get_hostname(tcpip_adapter_if_t tcpip_if, const char **hostname)
{
    return esp_netif_get_hostname(netif_from_if(tcpip_if), hostname);
}

#else

esp_err_t tcpip_adapter_compat_start_eth(void* eth_driver)
{
    return ESP_OK;
}

#endif