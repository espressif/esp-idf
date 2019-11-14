// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include <string.h>
#include "esp_netif.h"
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

    memset(netif_sta_list, 0, sizeof(esp_netif_sta_list_t));
    netif_sta_list->num = wifi_sta_list->num;
    for (int i = 0; i < wifi_sta_list->num; i++) {
        memcpy(netif_sta_list->sta[i].mac, wifi_sta_list->sta[i].mac, 6);
        dhcp_search_ip_on_mac(netif_sta_list->sta[i].mac, (ip4_addr_t*)&netif_sta_list->sta[i].ip);
    }

    return ESP_OK;
}

#endif // CONFIG_ESP_NETIF_TCPIP_LWIP
