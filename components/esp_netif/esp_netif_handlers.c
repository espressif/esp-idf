/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
    ESP_LOGD(TAG, "esp_netif action has started with netif%p from event_id=%" PRId32, esp_netif, event_id);
    esp_netif_start(esp_netif);
}

void esp_netif_action_stop(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{
    ESP_LOGD(TAG, "esp_netif action stopped with netif%p from event_id=%" PRId32, esp_netif, event_id);
    esp_netif_stop(esp_netif);
}

void esp_netif_action_connected(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{

    ESP_LOGD(TAG, "esp_netif action connected with netif%p from event_id=%" PRId32, esp_netif, event_id);
    esp_netif_up(esp_netif);

    if (!(esp_netif_get_flags(esp_netif) & ESP_NETIF_DHCP_CLIENT)) {
        // No more actions for interfaces without DHCP client flag
        return;
    }
#if CONFIG_LWIP_IPV4
    esp_netif_dhcp_status_t status;
    ESP_NETIF_CALL_CHECK("connected action: dhcpc failed", esp_netif_dhcpc_get_status(esp_netif, &status), ESP_OK);
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
                    .ip_changed = false,
            };

            if (memcmp(&ip, &old_ip, sizeof(ip))) {
                evt.ip_changed = true;
            }

            memcpy(&evt.ip_info, &ip, sizeof(esp_netif_ip_info_t));
            esp_netif_set_old_ip_info(esp_netif, &ip);

            ESP_NETIF_CALL_CHECK("esp_event_post in esp_netif_action_connected",
                    esp_event_post(IP_EVENT, esp_netif_get_event_id(esp_netif, ESP_NETIF_IP_EVENT_GOT_IP) ,
                                                    &evt, sizeof(evt), 0), ESP_OK);
            ESP_LOGD(TAG, "static ip: ip changed=%s", evt.ip_changed ? "true":"false");
        } else {
            ESP_LOGE(TAG, "invalid static ip");
        }
    }
#endif
}

void esp_netif_action_disconnected(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{
    ESP_LOGD(TAG, "esp_netif action disconnected with netif%p from event_id=%" PRId32, esp_netif, event_id);
    esp_netif_down(esp_netif);

}

void esp_netif_action_got_ip(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{
    ESP_LOGD(TAG, "esp_netif action got_ip with netif%p from event_id=%" PRId32, esp_netif, event_id);
    const ip_event_got_ip_t *event = (const ip_event_got_ip_t *) data;
    ESP_LOGI(TAG, "%s ip: " IPSTR ", mask: " IPSTR ", gw: " IPSTR, esp_netif_get_desc(esp_netif),
             IP2STR(&event->ip_info.ip),
             IP2STR(&event->ip_info.netmask),
             IP2STR(&event->ip_info.gw));
}

void esp_netif_action_join_ip6_multicast_group(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{
    ESP_LOGD(TAG, "esp_netif action join_ip6_multicast group with netif%p from event_id=%" PRId32, esp_netif, event_id);
    const esp_ip6_addr_t *addr = (const esp_ip6_addr_t *)data;
    esp_netif_join_ip6_multicast_group(esp_netif, addr);
}

void esp_netif_action_leave_ip6_multicast_group(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{
    ESP_LOGD(TAG, "esp_netif action leave_ip6_multicast_group with netif%p from event_id=%" PRId32, esp_netif, event_id);
    const esp_ip6_addr_t *addr = (const esp_ip6_addr_t *)data;
    esp_netif_leave_ip6_multicast_group(esp_netif, addr);
}

void esp_netif_action_add_ip6_address(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{
    ESP_LOGD(TAG, "esp_netif action add_ip6_address with netif%p from event_id=%" PRId32, esp_netif, event_id);
    const ip_event_add_ip6_t *addr = (const ip_event_add_ip6_t *)data;
    esp_netif_add_ip6_address(esp_netif, addr);
}

void esp_netif_action_remove_ip6_address(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{
    ESP_LOGD(TAG, "esp_netif action remove_ip6_address with netif%p from event_id=%" PRId32, esp_netif, event_id);
    const esp_ip6_addr_t *addr = (const esp_ip6_addr_t *)data;
    esp_netif_remove_ip6_address(esp_netif, addr);
}
