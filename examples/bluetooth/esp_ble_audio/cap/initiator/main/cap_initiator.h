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
#include "esp_ble_audio_bap_api.h"
#include "esp_ble_audio_cap_api.h"
#include "esp_ble_audio_pacs_api.h"
#include "esp_ble_audio_bap_lc3_preset_defs.h"

#include "ble_audio_example_init.h"
#include "ble_audio_example_utils.h"

#define TAG "CAP_INI"

#define CONN_HANDLE_INIT    0xFFFF

struct tx_stream {
    esp_ble_audio_cap_stream_t *stream;
    uint16_t seq_num;
    uint8_t *data;
    example_audio_tx_scheduler_t scheduler;
    bool is_broadcast;
};

void cap_initiator_unicast_gap_cb(esp_ble_audio_gap_app_event_t *event);

void cap_initiator_unicast_gatt_cb(esp_ble_audio_gatt_app_event_t *event);

int cap_initiator_unicast_start(void);

int cap_initiator_unicast_init(void);

int cap_initiator_broadcast_start(void);

int cap_initiator_broadcast_init(void);

void cap_initiator_tx_stream_sent(esp_ble_audio_bap_stream_t *stream, void *user_data);

int cap_initiator_tx_register_stream(esp_ble_audio_cap_stream_t *cap_stream, bool is_broadcast);

int cap_initiator_tx_unregister_stream(esp_ble_audio_cap_stream_t *cap_stream);

void cap_initiator_tx_init(void);
