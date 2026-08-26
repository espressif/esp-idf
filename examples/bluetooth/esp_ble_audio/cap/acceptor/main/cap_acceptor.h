/*
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "esp_log.h"

#include "sdkconfig.h"

#include "esp_ble_audio_lc3_defs.h"
#include "esp_ble_audio_cap_api.h"
#include "esp_ble_audio_pacs_api.h"

#include "ble_audio_example_init.h"
#include "ble_audio_example_utils.h"

#define TAG "CAP_ACC"

#define CONN_HANDLE_INIT    0xFFFF

#define LOCAL_DEVICE_NAME   "CAP Acceptor"

#define ADV_HANDLE          0
#define ADV_SID             0
#define ADV_TX_POWER        127
#define ADV_INTERVAL_MS     200

#define SCAN_INTERVAL       160     /* 100ms */
#define SCAN_WINDOW         160     /* 100ms */

#define PA_SYNC_SKIP        0
#define PA_SYNC_TIMEOUT     1000    /* 1000 * 10ms = 10s */

#define SINK_CONTEXT        (ESP_BLE_AUDIO_CONTEXT_TYPE_UNSPECIFIED | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_CONVERSATIONAL | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_GAME | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_INSTRUCTIONAL)
#define SOURCE_CONTEXT      (ESP_BLE_AUDIO_CONTEXT_TYPE_UNSPECIFIED | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_CONVERSATIONAL | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_GAME)

/* Number of stream objects the application backs per direction.
 *
 * Sink: one per exposed ASE, so a stereo Initiator can configure two independent
 * sink streams. The CAP handover procedures hand over all streaming sink streams
 * at once (CAP 7.3.1.10 applies to every CIS carrying Initiator-to-Acceptor audio),
 * so the sink pool has to match the number of ASEs that can be configured.
 *
 * Source: one. Only the bidirectional (call) configuration uses it, and the TX
 * pump in cap_acceptor_unicast.c drives a single stream. A Config request beyond
 * these counts is answered with NO_MEM by stream_alloc().
 */
#define ACCEPTOR_SINK_STREAM_COUNT      CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT
#define ACCEPTOR_SOURCE_STREAM_COUNT    1

struct peer_config {
    esp_ble_audio_cap_stream_t source_streams[ACCEPTOR_SOURCE_STREAM_COUNT];
    esp_ble_audio_cap_stream_t sink_streams[ACCEPTOR_SINK_STREAM_COUNT];
    uint16_t conn_handle;
    /* Assistant's BD addr — used by Bluedroid pa_sync_with_past to set per-peer
     * PAST receive params. Populated at ACL_CONNECT, cleared at ACL_DISCONNECT. */
    uint8_t dst[6];
};

extern struct peer_config peer;

int app_host_init(void);

int set_device_name(void);

int ext_adv_start(const uint8_t *ext_data, uint8_t ext_len);

#if CONFIG_EXAMPLE_SCAN_SELF
int ext_scan_start(void);
int ext_scan_stop(void);
#endif /* CONFIG_EXAMPLE_SCAN_SELF */

int pa_sync_create(uint8_t addr_type, const uint8_t addr[6], uint8_t sid);

int pa_sync_with_past(uint16_t conn_handle, uint8_t addr_type, const uint8_t addr[6]);

int pa_past_cancel(uint16_t conn_handle);

int pa_sync_terminate(uint16_t sync_handle);

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

void broadcast_pa_lost(uint16_t sync_handle);
#endif /* CONFIG_EXAMPLE_BROADCAST */
