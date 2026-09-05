/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#include "ble_audio_example_utils.h"

#define TAG "BAP_BSNK"

#define LOCAL_DEVICE_NAME       "BAP Broadcast Sink"

#define SCAN_INTERVAL           160     /* 100ms */
#define SCAN_WINDOW             160     /* 100ms */

#define PA_SYNC_SKIP            0
#define PA_SYNC_TIMEOUT         1000    /* 1000 * 10ms = 10s */

#define ADV_HANDLE              0
#define ADV_SID                 0
#define ADV_TX_POWER            127
#define ADV_INTERVAL_MS         200

int app_host_init(void);

int ext_scan_start(void);
int ext_scan_stop(void);

int pa_sync_create(uint8_t addr_type, const uint8_t addr[6], uint8_t sid);
int pa_sync_terminate(uint16_t sync_handle);

#if CONFIG_EXAMPLE_SCAN_OFFLOAD
int set_device_name(void);

int ext_adv_start(const uint8_t *ext_data, uint8_t ext_len);
#endif /* CONFIG_EXAMPLE_SCAN_OFFLOAD */

#if CONFIG_EXAMPLE_PAST
int pa_sync_with_past(uint16_t conn_handle, const uint8_t addr[6]);
#endif /* CONFIG_EXAMPLE_PAST */
