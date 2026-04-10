/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BTDM_LP_H_
#define _BTDM_LP_H_

void btdm_lp_enable_clock(esp_btdm_controller_config_t *cfg);

void btdm_lp_disable_clock(void);

int btdm_lp_init(void);

void btdm_lp_deinit(void);

void btdm_lp_reset(bool enable_stage);

void btdm_lp_shutdown(void);

#endif
