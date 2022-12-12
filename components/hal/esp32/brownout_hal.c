
/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/brownout_hal.h"
#include "soc/rtc_cntl_struct.h"
#include "esp_attr.h"

void brownout_hal_config(const brownout_hal_config_t *cfg)
{
    typeof(RTCCNTL.brown_out) brown_out_reg = {
        .close_flash_ena = cfg->flash_power_down,
        .pd_rf_ena = cfg->rf_power_down,
        .rst_wait = 0x3ff,
        .rst_ena = cfg->reset_enabled,
        .thres = cfg->threshold,
        .ena = cfg->enabled,
    };

    RTCCNTL.brown_out = brown_out_reg;
}

void brownout_hal_intr_enable(bool enable)
{
    RTCCNTL.int_ena.rtc_brown_out = enable;
}

IRAM_ATTR void brownout_hal_intr_clear(void)
{
    RTCCNTL.int_clr.rtc_brown_out = 1;
}
