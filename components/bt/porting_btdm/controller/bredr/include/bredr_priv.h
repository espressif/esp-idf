/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_BREDR_PRIV_H__
#define __ESP_BREDR_PRIV_H__
#include "btdm_osal.h"

int bredr_stack_init(esp_bt_controller_config_t *cfg);

void bredr_stack_deinit(void);

int bredr_stack_enable(void);

void bredr_stack_disable(void);

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
esp_err_t sleep_modem_bredr_mac_modem_state_init(void);
#endif // CONFIG_FREERTOS_USE_TICKLESS_IDLE

void bredr_osal_elem_calc(esp_bt_controller_config_t *cfg, btdm_osal_elem_num_t *elem);

#endif // __ESP_BREDR_PRIV_H__
