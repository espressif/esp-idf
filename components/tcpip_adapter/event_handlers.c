// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include "tcpip_adapter.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_private/wifi.h"
#include "esp_eth.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "tcpip_adapter";

#define API_CALL_CHECK(info, api_call, ret) \
do{\
    esp_err_t __err = (api_call);\
    if ((ret) != __err) {\
        ESP_LOGE(TAG, "%s %d %s ret=0x%X", __FUNCTION__, __LINE__, (info), __err);\
        return;\
    }\
} while(0)

static void handle_ap_start(void *arg, esp_event_base_t base, int32_t event_id, void *data);
static void handle_ap_stop(void *arg, esp_event_base_t base, int32_t event_id, void *data);
static void handle_sta_start(void *arg, esp_event_base_t base, int32_t event_id, void *data);
static void handle_sta_stop(void *arg, esp_event_base_t base, int32_t event_id, void *data);
static void handle_sta_connected(void *arg, esp_event_base_t base, int32_t event_id, void *data);
static void handle_sta_disconnected(void *arg, esp_event_base_t base, int32_t event_id, void *data);
static void handle_sta_got_ip(void *arg, esp_event_base_t base, int32_t event_id, void *data);

static void handle_eth_start(void *arg, esp_event_base_t base, int32_t event_id, void *data);
static void handle_eth_stop(void *arg, esp_event_base_t base, int32_t event_id, void *data);
static void handle_eth_connected(void *arg, esp_event_base_t base, int32_t event_id, void *data);
static void handle_eth_disconnected(void *arg, esp_event_base_t base, int32_t event_id, void *data);
static void handle_eth_got_ip(void *arg, esp_event_base_t base, int32_t event_id, void *data);

static void handle_eth_start(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    tcpip_adapter_ip_info_t eth_ip;
    uint8_t eth_mac[6];
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t*)data;
    esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, eth_mac);
    tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_ETH, &eth_ip);
    tcpip_adapter_eth_start(eth_mac, &eth_ip, eth_handle);
}

static void handle_eth_stop(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    tcpip_adapter_stop(TCPIP_ADAPTER_IF_ETH);
}

static void handle_eth_connected(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    tcpip_adapter_dhcp_status_t status;

    tcpip_adapter_up(TCPIP_ADAPTER_IF_ETH);

    tcpip_adapter_dhcpc_get_status(TCPIP_ADAPTER_IF_ETH, &status);

    if (status == TCPIP_ADAPTER_DHCP_INIT) {
        tcpip_adapter_dhcpc_start(TCPIP_ADAPTER_IF_ETH);
    } else if (status == TCPIP_ADAPTER_DHCP_STOPPED) {
        tcpip_adapter_ip_info_t eth_ip;

        tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_ETH, &eth_ip);

        if (!(ip4_addr_isany_val(eth_ip.ip) || ip4_addr_isany_val(eth_ip.netmask))) {
            ip_event_got_ip_t evt;

            //notify event
            evt.if_index = TCPIP_ADAPTER_IF_ETH;
            memcpy(&evt.ip_info, &eth_ip, sizeof(tcpip_adapter_ip_info_t));
            API_CALL_CHECK("handle_eth_connected", esp_event_send_internal(IP_EVENT, IP_EVENT_ETH_GOT_IP, &evt, sizeof(evt), 0), ESP_OK);
        } else {
            ESP_LOGE(TAG, "invalid static ip");
        }
    }
}

static void handle_eth_disconnected(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    tcpip_adapter_down(TCPIP_ADAPTER_IF_ETH);
}

static void handle_sta_got_ip(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    API_CALL_CHECK("esp_wifi_internal_set_sta_ip", esp_wifi_internal_set_sta_ip(), ESP_OK);

    const ip_event_got_ip_t *event = (const ip_event_got_ip_t *) data;
    ESP_LOGI(TAG, "sta ip: " IPSTR ", mask: " IPSTR ", gw: " IPSTR,
             IP2STR(&event->ip_info.ip),
             IP2STR(&event->ip_info.netmask),
             IP2STR(&event->ip_info.gw));
}

static void handle_eth_got_ip(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    const ip_event_got_ip_t *event = (const ip_event_got_ip_t *) data;
    ESP_LOGI(TAG, "eth ip: " IPSTR ", mask: " IPSTR ", gw: " IPSTR,
             IP2STR(&event->ip_info.ip),
             IP2STR(&event->ip_info.netmask),
             IP2STR(&event->ip_info.gw));
}

static void handle_ap_start(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    tcpip_adapter_ip_info_t ap_ip;
    uint8_t ap_mac[6];

    API_CALL_CHECK("esp_wifi_internal_reg_rxcb", esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_AP, (wifi_rxcb_t)tcpip_adapter_ap_input), ESP_OK);
    API_CALL_CHECK("esp_wifi_mac_get",  esp_wifi_get_mac(ESP_IF_WIFI_AP, ap_mac), ESP_OK);

    tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &ap_ip);
    tcpip_adapter_ap_start(ap_mac, &ap_ip);
}

static void handle_ap_stop(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    API_CALL_CHECK("esp_wifi_internal_reg_rxcb", esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_AP, NULL), ESP_OK);

    tcpip_adapter_stop(TCPIP_ADAPTER_IF_AP);
}

static void handle_sta_start(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    tcpip_adapter_ip_info_t sta_ip;
    uint8_t sta_mac[6];

    API_CALL_CHECK("esp_wifi_mac_get",  esp_wifi_get_mac(ESP_IF_WIFI_STA, sta_mac), ESP_OK);
    tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &sta_ip);
    tcpip_adapter_sta_start(sta_mac, &sta_ip);
}

static void handle_sta_stop(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    tcpip_adapter_stop(TCPIP_ADAPTER_IF_STA);
}

static void handle_sta_connected(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    tcpip_adapter_dhcp_status_t status;

    API_CALL_CHECK("esp_wifi_internal_reg_rxcb", esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, (wifi_rxcb_t)tcpip_adapter_sta_input), ESP_OK);

    tcpip_adapter_up(TCPIP_ADAPTER_IF_STA);

    tcpip_adapter_dhcpc_get_status(TCPIP_ADAPTER_IF_STA, &status);

    if (status == TCPIP_ADAPTER_DHCP_INIT) {
        tcpip_adapter_dhcpc_start(TCPIP_ADAPTER_IF_STA);
    } else if (status == TCPIP_ADAPTER_DHCP_STOPPED) {
        tcpip_adapter_ip_info_t sta_ip;
        tcpip_adapter_ip_info_t sta_old_ip;

        tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &sta_ip);
        tcpip_adapter_get_old_ip_info(TCPIP_ADAPTER_IF_STA, &sta_old_ip);

        if (!(ip4_addr_isany_val(sta_ip.ip) || ip4_addr_isany_val(sta_ip.netmask))) {
            
            ip_event_got_ip_t evt;

            evt.if_index = TCPIP_ADAPTER_IF_STA;
            evt.ip_changed = false;

            if (memcmp(&sta_ip, &sta_old_ip, sizeof(sta_ip))) {
                evt.ip_changed = true;
            }

            memcpy(&evt.ip_info, &sta_ip, sizeof(tcpip_adapter_ip_info_t));
            tcpip_adapter_set_old_ip_info(TCPIP_ADAPTER_IF_STA, &sta_ip);

            API_CALL_CHECK("handle_sta_connected", esp_event_send_internal(IP_EVENT, IP_EVENT_STA_GOT_IP, &evt, sizeof(evt), 0), ESP_OK);
            ESP_LOGD(TAG, "static ip: ip changed=%d", evt.ip_changed);
        } else {
            ESP_LOGE(TAG, "invalid static ip");
        }
    }
}

static void handle_sta_disconnected(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    tcpip_adapter_down(TCPIP_ADAPTER_IF_STA);
    API_CALL_CHECK("esp_wifi_internal_reg_rxcb", esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, NULL), ESP_OK);
}


esp_err_t tcpip_adapter_set_default_wifi_handlers(void)
{
    esp_err_t err;
    err = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_START, handle_sta_start, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_STOP, handle_sta_stop, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, handle_sta_connected, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, handle_sta_disconnected, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_START, handle_ap_start, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_STOP, handle_ap_stop, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, handle_sta_got_ip, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_register_shutdown_handler((shutdown_handler_t)esp_wifi_stop);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        goto fail;
    }

    return ESP_OK;

fail:
    tcpip_adapter_clear_default_wifi_handlers();
    return err;
}

esp_err_t tcpip_adapter_clear_default_wifi_handlers(void)
{
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_START, handle_sta_start);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_STOP, handle_sta_stop);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, handle_sta_connected);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, handle_sta_disconnected);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_START, handle_ap_start);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_STOP, handle_ap_stop);
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, handle_sta_got_ip);
    esp_unregister_shutdown_handler((shutdown_handler_t)esp_wifi_stop);

    return ESP_OK;
}

esp_err_t tcpip_adapter_set_default_eth_handlers(void)
{
    esp_err_t err;
    err = esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_START, handle_eth_start, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_STOP, handle_eth_stop, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_CONNECTED, handle_eth_connected, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, handle_eth_disconnected, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, handle_eth_got_ip, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    return ESP_OK;

fail:
    tcpip_adapter_clear_default_eth_handlers();
    return err;
}

esp_err_t tcpip_adapter_clear_default_eth_handlers(void)
{
    esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_START, handle_eth_start);
    esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_STOP, handle_eth_stop);
    esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_CONNECTED, handle_eth_connected);
    esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, handle_eth_disconnected);
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, handle_eth_got_ip);
    return ESP_OK;
}
