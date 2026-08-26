/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#include "ble_iso_example_utils.h"

#define TAG "BIG_SNC"

#define SCAN_INTERVAL           160     /* 100ms */
#define SCAN_WINDOW             160     /* 100ms */

#define PA_SYNC_SKIP            0
#define PA_SYNC_TIMEOUT         1000    /* 1000 * 10ms = 10s */

int app_host_init(void);

int ext_scan_start(void);
int ext_scan_stop(void);

int pa_sync_create(uint8_t addr_type, const uint8_t addr[6], uint8_t sid);
