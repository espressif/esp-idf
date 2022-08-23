/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi_ap_get_sta_list.h"


esp_err_t esp_wifi_ap_get_sta_list_with_ip(const wifi_sta_list_t *wifi_sta_list, wifi_sta_mac_ip_list_t *wifi_sta_ip_mac_list)
{
    if ((wifi_sta_list == NULL) || (wifi_sta_ip_mac_list == NULL)) {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }
    esp_netif_t *ap = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
    if (ap == NULL) {
        return ESP_ERR_ESP_NETIF_IF_NOT_READY;
    }
    int num = wifi_sta_list->num;
    wifi_sta_ip_mac_list->num = num;
    for (int i = 0; i < num; i++) {
        memcpy(wifi_sta_ip_mac_list->sta[i].mac, wifi_sta_list->sta[i].mac, 6);
        memset(&wifi_sta_ip_mac_list->sta[i].ip, 0, sizeof(esp_ip4_addr_t));
    }
    return esp_netif_dhcps_get_clients_by_mac(ap, num, wifi_sta_ip_mac_list->sta);
}
