/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BTDM_LP_H_
#define _BTDM_LP_H_
#include "esp_private/esp_modem_clock.h"

void btdm_lp_enable_clock(esp_btdm_controller_config_t *cfg);

void btdm_lp_disable_clock(void);

int btdm_lp_init(void);

void btdm_lp_deinit(void);

void btdm_lp_reset(bool enable_stage);

void btdm_lp_shutdown(void);

modem_clock_lpclk_src_t btdm_lp_get_lpclk_src(void);

void btdm_lp_set_lpclk_src(modem_clock_lpclk_src_t clk_src);

uint32_t btdm_lp_get_lpclk_freq(void);

void btdm_lp_set_lpclk_freq(uint32_t clk_freq);


#endif
