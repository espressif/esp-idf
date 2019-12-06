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
#include "esp_log.h"
#include "esp_event.h"
#include "esp_event_legacy.h"

#define TAG "event_send"

esp_err_t esp_event_send_noop(system_event_t *event);
extern esp_err_t esp_event_send_legacy(system_event_t *event) __attribute__((weak, alias("esp_event_send_noop")));
extern esp_err_t esp_event_send_to_default_loop(system_event_t *event) __attribute((weak, alias("esp_event_send_noop")));

esp_err_t esp_event_send_noop(system_event_t *event)
{
    return ESP_OK;
}

static system_event_id_t esp_event_legacy_wifi_event_id(int32_t event_id)
{
    switch (event_id) {
    case WIFI_EVENT_WIFI_READY:
        return SYSTEM_EVENT_WIFI_READY;

    case WIFI_EVENT_SCAN_DONE:
        return SYSTEM_EVENT_SCAN_DONE;

    case WIFI_EVENT_STA_START:
        return SYSTEM_EVENT_STA_START;

    case WIFI_EVENT_STA_STOP:
        return SYSTEM_EVENT_STA_STOP;

    case WIFI_EVENT_STA_CONNECTED:
        return SYSTEM_EVENT_STA_CONNECTED;

    case WIFI_EVENT_STA_DISCONNECTED:
        return SYSTEM_EVENT_STA_DISCONNECTED;

    case WIFI_EVENT_STA_AUTHMODE_CHANGE:
        return SYSTEM_EVENT_STA_AUTHMODE_CHANGE;

    case WIFI_EVENT_STA_WPS_ER_SUCCESS:
        return SYSTEM_EVENT_STA_WPS_ER_SUCCESS;

    case WIFI_EVENT_STA_WPS_ER_FAILED:
        return SYSTEM_EVENT_STA_WPS_ER_FAILED;

    case WIFI_EVENT_STA_WPS_ER_TIMEOUT:
        return SYSTEM_EVENT_STA_WPS_ER_TIMEOUT;

    case WIFI_EVENT_STA_WPS_ER_PIN:
        return SYSTEM_EVENT_STA_WPS_ER_PIN;

    case WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP:
        return SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP;

    case WIFI_EVENT_AP_START:
        return SYSTEM_EVENT_AP_START;

    case WIFI_EVENT_AP_STOP:
        return SYSTEM_EVENT_AP_STOP;

    case WIFI_EVENT_AP_STACONNECTED:
        return SYSTEM_EVENT_AP_STACONNECTED;

    case WIFI_EVENT_AP_STADISCONNECTED:
        return SYSTEM_EVENT_AP_STADISCONNECTED;

    case WIFI_EVENT_AP_PROBEREQRECVED:
        return SYSTEM_EVENT_AP_PROBEREQRECVED;

    default:
        ESP_LOGE(TAG, "invalid wifi event id %d", event_id);
        return SYSTEM_EVENT_MAX;
    }
}

static system_event_id_t esp_event_legacy_ip_event_id(int32_t event_id)
{
    switch (event_id) {
    case IP_EVENT_STA_GOT_IP:
        return SYSTEM_EVENT_STA_GOT_IP;

    case IP_EVENT_STA_LOST_IP:
        return SYSTEM_EVENT_STA_LOST_IP;

    case IP_EVENT_AP_STAIPASSIGNED:
        return SYSTEM_EVENT_AP_STAIPASSIGNED;

    case IP_EVENT_GOT_IP6:
        return SYSTEM_EVENT_GOT_IP6;

    case IP_EVENT_ETH_GOT_IP:
        return SYSTEM_EVENT_ETH_GOT_IP;

    default:
        ESP_LOGE(TAG, "invalid ip event id %d", event_id);
        return SYSTEM_EVENT_MAX;
    }
}


static system_event_id_t esp_event_legacy_event_id(esp_event_base_t event_base, int32_t event_id)
{
    if (event_base == WIFI_EVENT) {
        return esp_event_legacy_wifi_event_id(event_id);
    } else if (event_base == IP_EVENT) {
        return esp_event_legacy_ip_event_id(event_id);
    } else {
        ESP_LOGE(TAG, "invalid event base %s", event_base);
        return SYSTEM_EVENT_MAX;
    }
}

esp_err_t esp_event_send(system_event_t *event)
{
    // send the event to the new style event loop
    esp_err_t err = esp_event_send_to_default_loop(event);
    if (err != ESP_OK) {
        return err;
    }

    // send the event to the legacy event loop
    err = esp_event_send_legacy(event);
    if (err != ESP_OK) {
        return err;
    }

    return ESP_OK;
}

esp_err_t esp_event_send_internal(esp_event_base_t event_base,
                            int32_t event_id,
                            void* event_data,
                            size_t event_data_size,
                            TickType_t ticks_to_wait)
{
    system_event_t event;

    // send the event to the new style event loop
    esp_err_t err = esp_event_post(event_base, event_id, event_data, event_data_size, ticks_to_wait);
    if (err != ESP_OK) {
        return err;
    }

    event.event_id = esp_event_legacy_event_id(event_base, event_id);

    if (event_data) {
        memcpy(&event.event_info, event_data, event_data_size);
    }

    return esp_event_send_legacy(&event);
}

