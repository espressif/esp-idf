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
#include "ble_log_lbm_v2.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/* MACRO */
#define BLE_LOG_TS_TRIGGER_TIMEOUT_MS           (1000)
#define BLE_LOG_TS_TRIGGER_TIMEOUT_US           (BLE_LOG_TS_TRIGGER_TIMEOUT_MS * 1000ULL)

/* INTERFACE */
bool ble_log_rt_init(void);
/* Starts the always-on periodic path after the epoch INIT frame is queued. */
bool ble_log_rt_start_periodic(void);
void ble_log_rt_deinit(void);
bool ble_log_rt_drain(void);
void ble_log_rt_submit_trans(ble_log_prph_trans_t *trans);

/* Samples the link-layer, ESP and OS clocks at one instant. toggle_io allows
 * the periodic path to toggle the sync IO when runtime IO toggling is enabled;
 * one-shot snapshots (INIT/FLUSH) always pass false and leave the IO edge
 * sequence untouched. io_level always reports the physical IO level. */
void ble_log_rt_ts_sample(ble_log_ts_info_t *info, bool toggle_io);

#endif /* __BLE_LOG_RT_H__ */
