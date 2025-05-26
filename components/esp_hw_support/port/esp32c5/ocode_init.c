/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "soc/rtc.h"
#include "esp_attr.h"
#include "soc/regi2c_dig_reg.h"
#include "soc/regi2c_lp_bias.h"
#include "hal/efuse_hal.h"
#include "hal/efuse_ll.h"
#include "hal/clk_tree_ll.h"
#include "esp_private/regi2c_ctrl.h"
#include "esp_hw_log.h"

static const char *TAG = "ocode_init";

static void set_ocode_by_efuse(int ocode_scheme_ver)
{
    assert(ocode_scheme_ver == 1);
    unsigned int ocode = efuse_ll_get_ocode();

    //set ext_ocode
    REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_EXT_CODE, ocode);
    REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_IR_FORCE_CODE, 1);
}

static void IRAM_ATTR NOINLINE_ATTR calibrate_ocode(void)
{
    /*
    Bandgap output voltage is not precise when calibrate o-code by hardware sometimes, so need software o-code calibration (must turn off PLL).
    Method:
    1. read current cpu config, save in old_config;
    2. switch cpu to xtal because PLL will be closed when o-code calibration;
    3. begin o-code calibration;
    4. wait o-code calibration done flag(odone_flag & bg_odone_flag) or timeout;
    5. set cpu to old-config.
    */
    soc_rtc_slow_clk_src_t slow_clk_src = rtc_clk_slow_src_get();
    rtc_cal_sel_t cal_clk = RTC_CAL_RTC_MUX;
    if (slow_clk_src == SOC_RTC_SLOW_CLK_SRC_OSC_SLOW) {
        cal_clk = RTC_CAL_32K_OSC_SLOW;
    } else if (slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
        cal_clk  = RTC_CAL_32K_XTAL;
    }

    uint64_t max_delay_time_us = 10000;
    uint32_t slow_clk_period = rtc_clk_cal(cal_clk, 100);
    uint64_t max_delay_cycle = rtc_time_us_to_slowclk(max_delay_time_us, slow_clk_period);
    uint64_t cycle0 = rtc_time_get();
    uint64_t timeout_cycle = cycle0 + max_delay_cycle;
    uint64_t cycle1 = 0;

    rtc_cpu_freq_config_t old_config;
    rtc_clk_cpu_freq_get_config(&old_config);
    rtc_clk_cpu_freq_set_xtal();

    ANALOG_CLOCK_ENABLE();
    REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_IR_RESETB, 0);
    REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_IR_RESETB, 1);
    bool odone_flag = 0;
    bool bg_odone_flag = 0;
    while (1) {
        odone_flag = REGI2C_READ_MASK(I2C_ULP, I2C_ULP_O_DONE_FLAG);
        bg_odone_flag = REGI2C_READ_MASK(I2C_ULP, I2C_ULP_BG_O_DONE_FLAG);
        cycle1 = rtc_time_get();
        if (odone_flag && bg_odone_flag) {
            break;
        }
        if (cycle1 >= timeout_cycle) {
            ESP_HW_LOGW(TAG, "o_code calibration fail\n");
            break;
        }
    }
    ANALOG_CLOCK_DISABLE();

    rtc_clk_cpu_freq_set_config(&old_config);
}

void esp_ocode_calib_init(void)
{
    uint32_t chip_version = efuse_hal_chip_revision();
    uint32_t blk_ver = efuse_hal_blk_version();
    if ((chip_version == 1 && blk_ver >= 1) || (chip_version >= 100 && blk_ver >= 2)) {
        set_ocode_by_efuse(1);
        ESP_HW_LOGD(TAG, "efuse ocode");
    } else {
        calibrate_ocode();
        ESP_HW_LOGD(TAG, "calib ocode");
    }
}
