/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "esp_log.h"

#include "sdkconfig.h"

#include "esp_ble_audio_bap_api.h"
#include "esp_ble_audio_cap_api.h"
#include "esp_ble_audio_pacs_api.h"
#include "esp_ble_audio_tmap_api.h"
#include "esp_ble_audio_bap_lc3_preset_defs.h"

#include "ble_audio_example_init.h"
#include "ble_audio_example_utils.h"

#define TAG "TMAP_BMR"

#define SCAN_INTERVAL           160     /* 100ms */
#define SCAN_WINDOW             160     /* 100ms */

#define PA_SYNC_SKIP            0
#define PA_SYNC_TIMEOUT         1000    /* 1000 * 10ms = 10s */

int app_host_init(void);

int ext_scan_start(void);
int ext_scan_stop(void);

int pa_sync_create(uint8_t addr_type, const uint8_t addr[6], uint8_t sid);
int pa_sync_terminate(uint16_t sync_handle);

int vcp_vol_renderer_init(void);

void bap_broadcast_scan_recv(esp_ble_audio_gap_app_event_t *event);

void bap_broadcast_pa_sync(esp_ble_audio_gap_app_event_t *event);

void bap_broadcast_pa_lost(esp_ble_audio_gap_app_event_t *event);

int bap_broadcast_sink_scan(void);

int bap_broadcast_sink_init(void);
