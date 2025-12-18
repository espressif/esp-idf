/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BLE_LOG_TS_H__
#define __BLE_LOG_TS_H__

/* ----------------------------------- */
/* BLE Log - Timestamp Synchronization */
/* ----------------------------------- */

/* INCLUDE */
#include "ble_log_util.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "driver/gpio.h"

/* MACRO */
#if CONFIG_BLE_LOG_LL_ENABLED
/* ESP BLE Controller Gen 2 */
#if defined(CONFIG_IDF_TARGET_ESP32H2) || defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C5) ||\
    defined(CONFIG_IDF_TARGET_ESP32C61) || defined(CONFIG_IDF_TARGET_ESP32H21) || defined(CONFIG_IDF_TARGET_ESP32H4)
extern uint32_t r_ble_lll_timer_current_tick_get(void);
#define BLE_LOG_GET_LC_TS r_ble_lll_timer_current_tick_get()
/* ESP BLE Controller Gen 1 */
#elif defined(CONFIG_IDF_TARGET_ESP32C2)
extern uint32_t r_os_cputime_get32(void);
#define BLE_LOG_GET_LC_TS r_os_cputime_get32()
/* Legacy BLE Controller */
#elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S3)
extern uint32_t lld_read_clock_us(void);
#define BLE_LOG_GET_LC_TS lld_read_clock_us()
#else /* Other targets */
#define BLE_LOG_GET_LC_TS 0
#endif /* BLE targets */
#else /* !CONFIG_BLE_LOG_LL_ENABLED */
#define BLE_LOG_GET_LC_TS 0
#endif /* CONFIG_BLE_LOG_LL_ENABLED */

/* TYPEDEF */
typedef struct {
    uint8_t int_src_code;
    uint8_t io_level;
    uint32_t lc_ts;
    uint32_t esp_ts;
    uint32_t os_ts;
} __attribute__((packed)) ble_log_ts_info_t;

/* INTERFACE */
bool ble_log_ts_init(void);
void ble_log_ts_deinit(void);
void ble_log_ts_info_update(ble_log_ts_info_t **ts_info);

#endif /* __BLE_LOG_TS_H__ */
