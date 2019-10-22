// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_netif_private.h"

//
// Purpose of this module is to define general event handler primitives
// enabling application code to them either directly or with minimal modification
// for example with a separate pre/post handler.
// This module has no dependency on a specific network stack (lwip)
//

static const char *TAG = "esp_netif_handlers";

void esp_netif_action_start(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{
    ESP_LOGD(TAG, "esp_netif action has started with netif%p from event_id=%d", esp_netif, event_id);
    esp_netif_start(esp_netif);
}

void esp_netif_action_stop(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{
    ESP_LOGD(TAG, "esp_netif action stopped with netif%p from event_id=%d", esp_netif, event_id);
    esp_netif_stop(esp_netif);
}

void esp_netif_action_connected(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{
    esp_netif_dhcp_status_t status;

    ESP_LOGD(TAG, "esp_netif action connected with netif%p from event_id=%d", esp_netif, event_id);
    esp_netif_up(esp_netif);

    esp_netif_dhcpc_get_status(esp_netif, &status);
    if (status == ESP_NETIF_DHCP_INIT) {
        esp_netif_dhcpc_start(esp_netif);
    } else if (status == ESP_NETIF_DHCP_STOPPED) {
        //
        esp_netif_ip_info_t ip;
        esp_netif_ip_info_t old_ip;

        esp_netif_get_ip_info(esp_netif, &ip);
        esp_netif_get_old_ip_info(esp_netif, &old_ip);

        if (esp_netif_is_valid_static_ip(&ip)) {
            ip_event_got_ip_t evt = {
                    .esp_netif = esp_netif,
                    .if_index = -1, // to indicate ptr to if used
                    .ip_changed = false,
            };

            if (memcmp(&ip, &old_ip, sizeof(ip))) {
                evt.ip_changed = true;
            }

            memcpy(&evt.ip_info, &ip, sizeof(esp_netif_ip_info_t));
            esp_netif_set_old_ip_info(esp_netif, &ip);

            ESP_NETIF_CALL_CHECK("esp_event_send_internal in esp_netif_action_connected",
                    esp_event_send_internal(IP_EVENT, esp_netif_get_event_id(esp_netif, ESP_NETIF_IP_EVENT_GOT_IP) ,
                                                    &evt, sizeof(evt), 0), ESP_OK);
            ESP_LOGD(TAG, "static ip: ip changed=%d", evt.ip_changed);
        } else {
            ESP_LOGE(TAG, "invalid static ip");
        }
    }
}

void esp_netif_action_disconnected(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{
    ESP_LOGD(TAG, "esp_netif action disconnected with netif%p from event_id=%d", esp_netif, event_id);
    esp_netif_down(esp_netif);

}

void esp_netif_action_got_ip(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{
    ESP_LOGD(TAG, "esp_netif action got_ip with netif%p from event_id=%d", esp_netif, event_id);
    const ip_event_got_ip_t *event = (const ip_event_got_ip_t *) data;
    ESP_LOGI(TAG, "%s ip: " IPSTR ", mask: " IPSTR ", gw: " IPSTR, esp_netif_get_desc(esp_netif),
             IP2STR(&event->ip_info.ip),
             IP2STR(&event->ip_info.netmask),
             IP2STR(&event->ip_info.gw));
}

