/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#include "ble_iso_example_utils.h"

#define TAG "CIS_CEN"

#define LOCAL_DEVICE_NAME       "CIS Central"

#define SCAN_INTERVAL           160     /* 100ms */
#define SCAN_WINDOW             160     /* 100ms */

/* ACL init parameters shared between bluedroid and nimble host wrappers.
 * Raw HCI units (scan: 0.625ms; conn interval: 1.25ms; timeout: 10ms). */
#define INIT_SCAN_INTERVAL      16      /* 10ms */
#define INIT_SCAN_WINDOW        16      /* 10ms */
#define CONN_INTERVAL           24      /* 30ms */
#define CONN_LATENCY            0
#define CONN_TIMEOUT            500     /* 5s */
#define CONN_MIN_CE_LEN         0xFFFF
#define CONN_MAX_CE_LEN         0xFFFF

int app_host_init(void);

int set_device_name(void);

int ext_scan_start(void);
int ext_scan_stop(void);

int conn_create(uint8_t addr_type, const uint8_t addr[6]);

int pairing_start(uint16_t conn_handle);

/* Recover from an SMP failure on the given connection — typically deletes the
 * stale bond and terminates the link so the next reconnect runs fresh
 * pairing. Host-specific implementation. */
void security_failed_recover(uint16_t conn_handle, uint8_t status);
