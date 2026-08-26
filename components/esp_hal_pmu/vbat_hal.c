/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/assert.h"
#include "hal/vbat_ll.h"
#include "hal/vbat_hal.h"

void vbat_hal_config(const vbat_hal_config_t *cfg)
{
    vbat_ll_clear_count();
    vbat_ll_enable_charger_comparator(cfg->enable_vbat_charger);
    if (cfg->enable_vbat_charger) {
        uint8_t resistor_reg = (cfg->charger_resistor_value - 1000) / 500;
        vbat_ll_set_charger_resistor(resistor_reg);
        vbat_ll_set_charger_threshold(cfg->low_threshold, cfg->high_threshold);
        HAL_ASSERT(cfg->undervoltage_filter_time > cfg->upvoltage_filter_time);
        vbat_ll_set_undervoltage_filter_time(cfg->undervoltage_filter_time);
        vbat_ll_set_upvoltage_filter_time(cfg->upvoltage_filter_time);
    }
    vbat_ll_set_brownout_threshold(cfg->brownout_threshold);

    uint32_t int_status;
    vbat_ll_get_interrupt_status(&int_status);
    // TODO: When the chip wakes up from vbat under voltage, the upvoltage interrupt will
    //       be triggered at the same time, workaround by software here.
    if (int_status & VBAT_LL_CHARGER_UNDERVOLTAGE_INTR) {
        vbat_ll_clear_intr_mask(VBAT_LL_CHARGER_UPVOLTAGE_INTR);
    }

    vbat_ll_enable_intr_mask(cfg->interrupt_mask, true);
}
