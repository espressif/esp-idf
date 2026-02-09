/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __BREDR_APP_COMMON_UTILS_H__
#define __BREDR_APP_COMMON_UTILS_H__

#include <stdint.h>
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"

/**
 * @brief  Device event default handle function
 *
 * @param [in] event  event id
 * @param [in] param  handler parameter
 */
void bredr_app_dev_evt_def_hdl(esp_bt_dev_cb_event_t event, esp_bt_dev_cb_param_t *param);

/**
 * @brief  GAP event default handle function
 *
 * @param [in] event  event id
 * @param [in] param  handler parameter
 */
void bredr_app_gap_evt_def_hdl(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);

/**
 * @brief  Classic BT common initialization
 *
 * @return ESP_OK on successful init
 */
esp_err_t bredr_app_common_init(void);

#endif /* __BREDR_APP_COMMON_UTILS_H__*/
