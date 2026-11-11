/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_attr.h"
#include "soc/rtc.h"
#include "hal/efuse_hal.h"
#include "hal/efuse_ll.h"
#include "soc/regi2c_lp_bias.h"
#include "esp_private/regi2c_ctrl.h"
#include "esp_log.h"

ESP_LOG_ATTR_TAG_DRAM(TAG, "ocode_init");

/*
 * OCode initialization runs before PSRAM XIP is ready. In PSRAM XIP builds,
 * linker.lf keeps this file and its clock and REGI2C dependencies in internal
 * memory independently of the optional IRAM Kconfig settings.
 */
static void IRAM_ATTR set_ocode_by_efuse(void)
{
    unsigned int ocode = efuse_ll_get_ocode();

    REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_EXT_CODE, ocode);
    REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_IR_FORCE_CODE, 1);
}

static void IRAM_ATTR calibrate_ocode(void)
{
    const uint64_t max_delay_time_us = 10000;
    uint32_t slow_clk_period = rtc_clk_cal(CLK_CAL_RTC_SLOW, 100);
    uint64_t max_delay_cycle = rtc_time_us_to_slowclk(max_delay_time_us, slow_clk_period);
    uint64_t timeout_cycle = rtc_time_get() + max_delay_cycle;

    rtc_cpu_freq_config_t old_config;
    rtc_clk_cpu_freq_get_config(&old_config);
    rtc_clk_cpu_freq_set_xtal();

    ANALOG_CLOCK_ENABLE();
    REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_IR_RESETB, 0);
    REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_IR_RESETB, 1);
    while (!REGI2C_READ_MASK(I2C_ULP, I2C_ULP_O_DONE_FLAG) ||
            !REGI2C_READ_MASK(I2C_ULP, I2C_ULP_BG_O_DONE_FLAG)) {
        if (rtc_time_get() >= timeout_cycle) {
            ESP_DRAM_LOGW(TAG, "o_code calibration failed");
            break;
        }
    }
    ANALOG_CLOCK_DISABLE();

    rtc_clk_cpu_freq_set_config(&old_config);
}

void IRAM_ATTR esp_ocode_calib_init(void)
{
    uint32_t blk_version = efuse_hal_blk_version();
    if (blk_version >= 1 && blk_version < 5) {
        set_ocode_by_efuse();
        ESP_DRAM_LOGD(TAG, "efuse ocode");
    } else {
        calibrate_ocode();
        ESP_DRAM_LOGD(TAG, "software ocode");
    }
}
