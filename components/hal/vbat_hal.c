/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
        vbat_ll_set_undervoltage_filter_time(cfg->undervoltage_filter_time);
        vbat_ll_set_upvoltage_filter_time(cfg->upvoltage_filter_time);
    }
    vbat_ll_set_brownout_threshold(cfg->brownout_threshold);
    vbat_ll_enable_intr_mask(cfg->interrupt_mask, true);
}
