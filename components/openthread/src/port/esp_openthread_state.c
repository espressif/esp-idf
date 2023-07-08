/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_check.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_openthread_dns64.h>
#include <esp_openthread_netif_glue_priv.h>
#include <esp_openthread_state.h>
#include <lwip/dns.h>

#include <openthread/thread.h>

#define TAG "OT_STATE"

static void handle_ot_netif_state_change(otInstance* instance)
{
    if (otLinkIsEnabled(instance)) {
        ESP_LOGI(TAG, "netif up");
        if (esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_IF_UP, NULL, 0, 0) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to post OpenThread if up event");
        }
    } else {
        ESP_LOGI(TAG, "netif down");
        if (esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_IF_DOWN, NULL, 0, 0) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to post OpenThread if down event");
        }
    }
}

static void handle_ot_netdata_change(void)
{
#if CONFIG_OPENTHREAD_DNS64_CLIENT
    ip_addr_t dns_server_addr = *IP_ADDR_ANY;
    if (esp_openthread_get_nat64_prefix(&dns_server_addr.u_addr.ip6) == ESP_OK) {
        dns_server_addr.type = IPADDR_TYPE_V6;
        dns_server_addr.u_addr.ip6.addr[3] = ipaddr_addr(CONFIG_OPENTHREAD_DNS_SERVER_ADDR);
        const ip_addr_t* dnsserver = dns_getserver(OPENTHREAD_DNS_SERVER_INDEX);
        if (memcmp(dnsserver, &dns_server_addr, sizeof(ip_addr_t)) != 0) {
            ESP_LOGI(TAG, "Set dns server address: %s", ipaddr_ntoa(&dns_server_addr));
            dns_setserver(OPENTHREAD_DNS_SERVER_INDEX, &dns_server_addr);
            if (esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_SET_DNS_SERVER, NULL, 0, 0) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to post OpenThread set DNS server event");
            }
        }
    }
#endif
}

static void handle_ot_role_change(otInstance* instance)
{
    static otDeviceRole s_previous_role = OT_DEVICE_ROLE_DISABLED;
    otDeviceRole role = otThreadGetDeviceRole(instance);
    esp_err_t ret = ESP_OK;
    esp_openthread_role_changed_event_t event_data;
    event_data.current_role = role;
    event_data.previous_role = s_previous_role;
    if (esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_ROLE_CHANGED, &event_data, sizeof(event_data), 0) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to post OPENTHREAD_EVENT_ROLE_CHANGED");
    }
    if (role == OT_DEVICE_ROLE_CHILD || role == OT_DEVICE_ROLE_ROUTER || role == OT_DEVICE_ROLE_LEADER) {
        if (s_previous_role == OT_DEVICE_ROLE_DETACHED || s_previous_role == OT_DEVICE_ROLE_DISABLED) {
            otOperationalDataset dataset;
            ESP_GOTO_ON_FALSE(otDatasetGetActive(instance, &dataset) == OT_ERROR_NONE, ESP_FAIL, exit, TAG,
                "Failed to get the active dataset");
            ESP_GOTO_ON_ERROR(esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_ATTACHED, &dataset, sizeof(dataset), 0),
                exit, TAG, "Failed to post OPENTHREAD_EVENT_ATTACHED. Err: %s", esp_err_to_name(ret));
        }
    } else if (role == OT_DEVICE_ROLE_DETACHED) {
        if (s_previous_role != OT_DEVICE_ROLE_DISABLED) {
            ESP_GOTO_ON_ERROR(
                esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_DETACHED, &s_previous_role, sizeof(s_previous_role), 0),
                exit, TAG, "Failed to post OPENTHREAD_EVENT_DETACHED. Err: %s", esp_err_to_name(ret));
        }
    }
exit:
    s_previous_role = role;
}

static void ot_state_change_callback(otChangedFlags changed_flags, void* ctx)
{
    OT_UNUSED_VARIABLE(ctx);
    otInstance* instance = esp_openthread_get_instance();
    if (!instance) {
        return;
    }

    if (changed_flags & OT_CHANGED_THREAD_ROLE) {
        handle_ot_role_change(instance);
    }

    if (changed_flags & OT_CHANGED_THREAD_NETDATA) {
        handle_ot_netdata_change();
    }

    if (changed_flags & OT_CHANGED_THREAD_NETIF_STATE) {
        handle_ot_netif_state_change(instance);
    }
}

esp_err_t esp_openthread_state_event_init(otInstance* instance)
{
    ESP_RETURN_ON_FALSE(otSetStateChangedCallback(instance, ot_state_change_callback, NULL) == OT_ERROR_NONE,
        ESP_FAIL, TAG, "Failed to install state change callback");
    return ESP_OK;
}
