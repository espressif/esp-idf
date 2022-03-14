/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_netif_lwip_internal.h"
#include "esp_netif_sta_list.h"
#include "dhcpserver/dhcpserver.h"
#include "esp_log.h"

#if CONFIG_ESP_NETIF_TCPIP_LWIP

static const char *TAG = "esp_netif_sta_list";

esp_err_t esp_netif_get_sta_list(const wifi_sta_list_t *wifi_sta_list, esp_netif_sta_list_t *netif_sta_list)
{
    ESP_LOGD(TAG, "%s entered", __func__);

    if ((wifi_sta_list == NULL) || (netif_sta_list == NULL)) {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }
    esp_netif_t *ap = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
    if (ap == NULL) {
        return ESP_ERR_ESP_NETIF_IF_NOT_READY;
    }
    memset(netif_sta_list, 0, sizeof(esp_netif_sta_list_t));
    netif_sta_list->num = wifi_sta_list->num;
    for (int i = 0; i < wifi_sta_list->num; i++) {
        memcpy(netif_sta_list->sta[i].mac, wifi_sta_list->sta[i].mac, 6);
        dhcp_search_ip_on_mac(ap->dhcps, netif_sta_list->sta[i].mac, (ip4_addr_t*)&netif_sta_list->sta[i].ip);
    }

    return ESP_OK;
}

#endif // CONFIG_ESP_NETIF_TCPIP_LWIP
