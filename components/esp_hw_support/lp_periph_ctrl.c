/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "esp_private/lp_periph_ctrl.h"
#include "hal/lp_periph_clk_ctrl_ll.h"

static portMUX_TYPE lp_periph_spinlock = portMUX_INITIALIZER_UNLOCKED;

static uint8_t ref_counts[LP_PERIPH_MODULE_MAX] = {0};

void lp_periph_module_enable(lp_periph_module_t lp_periph)
{
    portENTER_CRITICAL(&lp_periph_spinlock);
    if (ref_counts[lp_periph] == 0) {
        // Enable clock and clear reset
        lp_periph_ll_enable_clk_clear_rst(lp_periph);
    }
    ref_counts[lp_periph]++;
    portEXIT_CRITICAL(&lp_periph_spinlock);
}

void lp_periph_module_disable(lp_periph_module_t lp_periph)
{
    portENTER_CRITICAL(&lp_periph_spinlock);
    ref_counts[lp_periph]--;
    if (ref_counts[lp_periph] == 0) {
        // Disable clock and set reset
        lp_periph_ll_disable_clk_set_rst(lp_periph);
    }
    portEXIT_CRITICAL(&lp_periph_spinlock);
}

void lp_periph_module_reset(lp_periph_module_t lp_periph)
{
    portENTER_CRITICAL(&lp_periph_spinlock);
    lp_periph_ll_reset(lp_periph);
    portEXIT_CRITICAL(&lp_periph_spinlock);
}

void lp_periph_set_clk_src(lp_periph_module_t lp_periph, soc_module_clk_t clk_src)
{
    portENTER_CRITICAL(&lp_periph_spinlock);
    lp_periph_ll_set_clk_src(lp_periph, clk_src);
    portEXIT_CRITICAL(&lp_periph_spinlock);
}
