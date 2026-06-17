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
#include "esp_ble_audio_bap_api.h"
#include "esp_ble_audio_pacs_api.h"
#include "esp_ble_audio_bap_lc3_preset_defs.h"
#include "esp_ble_audio_common_api.h"

#include "ble_audio_example_init.h"
#include "ble_audio_example_utils.h"

#define TAG "BAP_UCL"

struct tx_stream {
    esp_ble_audio_bap_stream_t *stream;
    uint16_t seq_num;
    uint8_t *data;
    example_audio_tx_scheduler_t scheduler;
};

void stream_tx_sent(esp_ble_audio_bap_stream_t *stream, void *user_data);

int stream_tx_register(esp_ble_audio_bap_stream_t *stream);

int stream_tx_unregister(esp_ble_audio_bap_stream_t *stream);

void stream_tx_init(void);
