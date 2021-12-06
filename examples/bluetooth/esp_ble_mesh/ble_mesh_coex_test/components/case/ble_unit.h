/*
 * ESP BLE Mesh Example
 *
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef _BLE_UNIT_H_
#define _BLE_UNIT_H_
#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gattc_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#define BLE_TC_SCAN_REPORT_PERIOD           10000

typedef struct {
    uint32_t adv_count;
    uint32_t scan_res_count;
} ble_util_scan_count_t;


extern esp_bd_addr_t bt_addr;
extern ble_util_scan_count_t scan_count;
extern esp_ble_adv_params_t default_adv_param;
extern esp_ble_scan_params_t default_scan_param;

void bt_test_init(void);
void bt_test_deinit(void);
void ble_gap_util_stop(void);

esp_err_t ble_gap_util_set_adv_data(uint8_t *adv_data, uint32_t adv_data_len, uint8_t *scan_rsp_data, uint32_t scan_rsp_data_len);
esp_err_t ble_gap_util_set_default_adv_data(void);
esp_err_t ble_gap_util_start_adv(esp_ble_adv_params_t *adv_param);
esp_err_t ble_gap_util_start_adv_default(void);
esp_err_t ble_gap_util_stop_adv(void);

esp_err_t ble_gap_util_set_scan_param(esp_ble_scan_params_t *scan_param);
esp_err_t ble_gap_util_set_default_scan_param(void);
esp_err_t ble_gap_util_start_scan(uint32_t duration);
esp_err_t ble_gap_util_stop_scan(void);

esp_err_t init_ble_gap_test_util(void);

#endif /* _BLE_UNIT_H_ */
