/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#include "ble_audio_example_utils.h"

#if CONFIG_BT_BLUEDROID_ENABLED
#define EXAMPLE_HOST_PAST_SEND  CONFIG_BT_BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER
#else
#define EXAMPLE_HOST_PAST_SEND  CONFIG_BT_NIMBLE_PERIODIC_ADV_SYNC_TRANSFER
#endif

/* The library offers PAST from the host feature alone, so a mismatch leaves the
 * delegator waiting for a transfer this build never sends. */
#if !CONFIG_EXAMPLE_PAST && EXAMPLE_HOST_PAST_SEND
#error "EXAMPLE_PAST=n also needs the host PAST-send feature off \
(CONFIG_BT_BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER / CONFIG_BT_NIMBLE_PERIODIC_ADV_SYNC_TRANSFER)"
#endif

#define TAG "BAP_BA"

#define LOCAL_DEVICE_NAME       "BAP Broadcast Assistant"

#define SCAN_INTERVAL           160     /* 100ms */
#define SCAN_WINDOW             160     /* 100ms */

#define INIT_SCAN_INTERVAL      16      /* 10ms */
#define INIT_SCAN_WINDOW        16      /* 10ms */
#define CONN_INTERVAL           24      /* 30ms */
#define CONN_LATENCY            0
#define CONN_TIMEOUT            500     /* 5s */
#define CONN_MIN_CE_LEN         0xFFFF
#define CONN_MAX_CE_LEN         0xFFFF

#define PA_SYNC_SKIP            0
#define PA_SYNC_TIMEOUT         1000    /* 1000 * 10ms = 10s */

int app_host_init(void);

int set_device_name(void);

int ext_scan_start(void);
int ext_scan_stop(void);

int conn_create(uint8_t addr_type, const uint8_t addr[6]);

int pairing_start(uint16_t conn_handle);

int exchange_mtu(uint16_t conn_handle);

void security_failed_recover(uint16_t conn_handle, uint8_t status);

int pa_sync_create(uint8_t addr_type, const uint8_t addr[6], uint8_t sid);
int pa_sync_terminate(uint16_t sync_handle);

int pa_past_transfer(uint16_t conn_handle, uint16_t sync_handle, uint8_t src_id);
