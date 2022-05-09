
/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/brownout_hal.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_attr.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_brownout.h"


void brownout_hal_config(const brownout_hal_config_t *cfg)
{
    REGI2C_WRITE_MASK(I2C_BOD, I2C_BOD_THRESHOLD, cfg->threshold);
    typeof(RTCCNTL.brown_out) brown_out_reg = {
        .close_flash_ena = cfg->flash_power_down,
        .pd_rf_ena = cfg->rf_power_down,
        .rst_wait = 0x3ff,
        .rst_ena = cfg->reset_enabled,
        .ena = cfg->enabled,
        .rst_sel = 1,
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
