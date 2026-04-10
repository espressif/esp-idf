/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _ESP_BLE_PRIV_H_
#define _ESP_BLE_PRIV_H_

#include "btdm_osal.h"

int ble_stack_init(esp_bt_controller_config_t *cfg);

void ble_stack_deinit(void);

int ble_stack_enable(void);

void ble_stack_disable(void);

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
esp_err_t sleep_modem_ble_mac_modem_state_init(void);
#endif // CONFIG_FREERTOS_USE_TICKLESS_IDLE

void ble_osal_elem_calc(esp_bt_controller_config_t *cfg, btdm_osal_elem_num_t *elem);

#endif // _ESP_BLE_PRIV_H_
