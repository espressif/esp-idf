/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_netif.h"
#include "esp_netif_types.h"
#include "esp_openthread_lock.h"
#include <esp_check.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_openthread_dns64.h>
#include <esp_openthread_netif_glue_priv.h>
#include <esp_openthread_radio.h>
#include <esp_openthread_state.h>

#include <openthread/thread.h>

#define TAG "OT_STATE"

static void handle_ot_netif_state_change(otInstance* instance)
{
    if (otIp6IsEnabled(instance)) {
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

#if (CONFIG_OPENTHREAD_RADIO_SPINEL_UART || CONFIG_OPENTHREAD_RADIO_SPINEL_SPI)
    esp_openthread_handle_netif_state_change(otIp6IsEnabled(instance));
#endif
}

static void handle_ot_netdata_change(void)
{
#if CONFIG_OPENTHREAD_DNS64_CLIENT
    ip6_addr_t dns_server_addr = *IP6_ADDR_ANY6;
    if (esp_openthread_get_nat64_prefix(&dns_server_addr) == ESP_OK) {
        dns_server_addr.addr[3] = ipaddr_addr(CONFIG_OPENTHREAD_DNS_SERVER_ADDR);
        ip6_addr_t current_dns_server_addr = *IP6_ADDR_ANY6;
        esp_openthread_task_switching_lock_release();
        esp_openthread_get_dnsserver_addr(&current_dns_server_addr);
        esp_openthread_task_switching_lock_acquire(portMAX_DELAY);
        if (memcmp(&current_dns_server_addr, &dns_server_addr, sizeof(ip6_addr_t)) != 0) {
            ESP_LOGI(TAG, "Set dns server address: %s", ip6addr_ntoa(&dns_server_addr));
            esp_openthread_task_switching_lock_release();
            if (esp_openthread_set_dnsserver_addr(dns_server_addr) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to set dns info for openthread netif");
                esp_openthread_task_switching_lock_acquire(portMAX_DELAY);
                return;
            }
            esp_openthread_task_switching_lock_acquire(portMAX_DELAY);
            if (esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_SET_DNS_SERVER, NULL, 0, 0) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to post OpenThread set DNS server event");
            }
        }
    }
#endif
}

static void handle_ot_role_change(otInstance* instance)
{
#if ((CONFIG_ESP_COEX_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE) && OPENTHREAD_RADIO_NATIVE)
        otLinkModeConfig linkmode = otThreadGetLinkMode(instance);
        esp_ieee802154_coex_config_t config = esp_openthread_get_coex_config();
        config.txrx = (linkmode.mRxOnWhenIdle) ? IEEE802154_LOW : IEEE802154_MIDDLE;
        esp_openthread_set_coex_config(config);
#endif
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
            ret = esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_ATTACHED, &dataset, sizeof(dataset), 0);
            ESP_GOTO_ON_ERROR(ret, exit, TAG, "Failed to post OPENTHREAD_EVENT_ATTACHED. Err: %s", esp_err_to_name(ret));
        }
    } else if (role == OT_DEVICE_ROLE_DETACHED) {
        if (s_previous_role != OT_DEVICE_ROLE_DISABLED) {
            ret = esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_DETACHED, &s_previous_role, sizeof(s_previous_role), 0);
            ESP_GOTO_ON_ERROR(ret, exit, TAG, "Failed to post OPENTHREAD_EVENT_DETACHED. Err: %s", esp_err_to_name(ret));
        }
    }
exit:
    s_previous_role = role;
}

static void handle_ot_dataset_change(esp_openthread_dataset_type_t type, otInstance *instance)
{
    esp_openthread_dataset_changed_event_t event_data;
    event_data.type = type;
    memset(&event_data.new_dataset, 0, sizeof(event_data.new_dataset));
    if (type == OPENTHREAD_ACTIVE_DATASET) {
        (void)otDatasetGetActive(instance, &event_data.new_dataset);
    } else if (type == OPENTHREAD_PENDING_DATASET) {
        (void)otDatasetGetPending(instance, &event_data.new_dataset);
    }
    if (esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_DATASET_CHANGED, &event_data, sizeof(event_data), 0) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to post dataset changed event");
    }
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

    if (changed_flags & OT_CHANGED_ACTIVE_DATASET) {
        handle_ot_dataset_change(OPENTHREAD_ACTIVE_DATASET, instance);
    }

    if (changed_flags & OT_CHANGED_PENDING_DATASET) {
        handle_ot_dataset_change(OPENTHREAD_PENDING_DATASET, instance);
    }
}

esp_err_t esp_openthread_state_event_init(otInstance* instance)
{
    ESP_RETURN_ON_FALSE(otSetStateChangedCallback(instance, ot_state_change_callback, NULL) == OT_ERROR_NONE,
        ESP_FAIL, TAG, "Failed to install state change callback");
    return ESP_OK;
}
