/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
#include "esp_task.h"

/* MACRO */
#define BLE_LOG_TASK_PRIO                       (ESP_TASK_PRIO_MAX - 1)
#define BLE_LOG_TASK_STACK_SIZE                 (1024)
#define BLE_LOG_TASK_HOOK_TIMEOUT_MS            (1000)

/* INTERFACE */
bool ble_log_rt_init();
void ble_log_rt_deinit(void);
void ble_log_rt_queue_trans(ble_log_prph_trans_t **trans);

#endif /* __BLE_LOG_RT_H__ */
