/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BLE_LOG_RT_H__
#define __BLE_LOG_RT_H__

/* ----------------- */
/* BLE Log - Runtime */
/* ----------------- */

/* INCLUDE */
#include "ble_log_prph.h"
#include "ble_log_ts.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/* MACRO */
#define BLE_LOG_TS_TRIGGER_TIMEOUT_MS           (1000)
#define BLE_LOG_TS_TRIGGER_TIMEOUT_US           (BLE_LOG_TS_TRIGGER_TIMEOUT_MS * 1000ULL)

/* INTERFACE */
bool ble_log_rt_init(void);
void ble_log_rt_deinit(void);
bool ble_log_rt_drain(void);
void ble_log_rt_submit_trans(ble_log_prph_trans_t *trans);

#endif /* __BLE_LOG_RT_H__ */
