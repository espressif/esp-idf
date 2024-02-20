
/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/brownout_ll.h"
#include "hal/brownout_hal.h"

void brownout_hal_config(const brownout_hal_config_t *cfg)
{
    // If brownout software control is enabled, hw ana reset should be disabled, because it always has the highest priority.
    brownout_ll_ana_reset_enable(false);
    brownout_ll_set_intr_wait_cycles(2);
    brownout_ll_enable_flash_power_down(cfg->flash_power_down);
    brownout_ll_enable_rf_power_down(cfg->rf_power_down);
    brownout_ll_clear_count();
    brownout_ll_reset_config(cfg->reset_enabled, 0x3ff, 1);
    brownout_ll_set_threshold(cfg->threshold);
    brownout_ll_bod_enable(cfg->enabled);
}
