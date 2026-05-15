/*
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"

#include "sdkconfig.h"

#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"

#include "esp_ble_audio_lc3_defs.h"
#include "esp_ble_audio_cap_api.h"
#include "esp_ble_audio_pacs_api.h"

#include "ble_audio_example_init.h"
#include "ble_audio_example_utils.h"

#define TAG "CAP_ACC"

#define CONN_HANDLE_INIT    0xFFFF

#define SINK_CONTEXT        (ESP_BLE_AUDIO_CONTEXT_TYPE_UNSPECIFIED | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_CONVERSATIONAL | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_GAME | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_INSTRUCTIONAL)
#define SOURCE_CONTEXT      (ESP_BLE_AUDIO_CONTEXT_TYPE_UNSPECIFIED | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_CONVERSATIONAL | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_GAME)

struct peer_config {
    esp_ble_audio_cap_stream_t source_stream;
    esp_ble_audio_cap_stream_t sink_stream;
    uint16_t conn_handle;
};

int cap_acceptor_unicast_init(struct peer_config *peer);

int cap_acceptor_broadcast_init(void);

esp_ble_audio_cap_stream_t *stream_alloc(esp_ble_audio_dir_t dir);

void stream_released(const esp_ble_audio_cap_stream_t *cap_stream);

#if CONFIG_EXAMPLE_BROADCAST
#if CONFIG_EXAMPLE_SCAN_SELF
int check_start_scan(void);

void broadcast_scan_recv(esp_ble_audio_gap_app_event_t *event);
#endif /* CONFIG_EXAMPLE_SCAN_SELF */

void broadcast_pa_synced(esp_ble_audio_gap_app_event_t *event);

void broadcast_pa_sync_failed(esp_ble_audio_gap_app_event_t *event);

void broadcast_pa_lost(esp_ble_audio_gap_app_event_t *event);
#endif /* CONFIG_EXAMPLE_BROADCAST */
