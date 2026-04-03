/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#if !CONFIG_IDF_TARGET_ESP32 && !CONFIG_IDF_TARGET_ESP32S2
#include "hal/psram_ctrlr_ll.h"
#endif
#if PSRAM_CTRLR_LL_DEDICATED_LDO
#include "hal/ldo_ll.h"
#include "esp_private/esp_psram_ldo.h"

void esp_psram_power_init(const esp_psram_power_cfg_t *config)
{
    uint8_t dref = 0;
    uint8_t mul = 0;
    bool use_rail_voltage = false;
    ldo_ll_enable_current_limit(0, true);
    ldo_ll_voltage_to_dref_mul(0, config->voltage_mv, &dref, &mul, &use_rail_voltage);
    ldo_ll_adjust_voltage(0, dref, mul, use_rail_voltage);
    ldo_ll_psram_power_enable(true);
    ldo_ll_enable_current_limit(0, false);
}

#endif
