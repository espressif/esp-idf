// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "esp32/rom/ets_sys.h"
#include "esp32/rom/rtc.h"
#include "esp32/rom/uart.h"
#include "esp32/rom/gpio.h"
#include "soc/rtc.h"
#include "soc/rtc_periph.h"
#include "soc/sens_periph.h"
#include "soc/efuse_periph.h"
#include "soc/apb_ctrl_reg.h"
#include "i2c_rtc_clk.h"
#include "soc_log.h"
#include "sdkconfig.h"
#include "xtensa/core-macros.h"
#include "rtc_clk_common.h"

/* Number of 8M/256 clock cycles to use for XTAL frequency estimation.
 * 10 cycles will take approximately 300 microseconds.
 */
#define XTAL_FREQ_EST_CYCLES            10

static rtc_xtal_freq_t rtc_clk_xtal_freq_estimate();

static const char* TAG = "rtc_clk_init";

void rtc_clk_init(rtc_clk_config_t cfg)
{
    rtc_cpu_freq_config_t old_config, new_config;

    /* If we get a TG WDT system reset while running at 240MHz,
     * DPORT_CPUPERIOD_SEL register will be reset to 0 resulting in 120MHz
     * APB and CPU frequencies after reset. This will cause issues with XTAL
     * frequency estimation, so we switch to XTAL frequency first.
     *
     * Ideally we would only do this if RTC_CNTL_SOC_CLK_SEL == PLL and
     * PLL is configured for 480M, but it takes less time to switch to 40M and
     * run the following code than querying the PLL does.
     */
    if (REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL) == RTC_CNTL_SOC_CLK_SEL_PLL) {
        /* We don't know actual XTAL frequency yet, assume 40MHz.
         * REF_TICK divider will be corrected below, once XTAL frequency is
         * determined.
         */
        rtc_clk_cpu_freq_to_xtal(40, 1);
    }

    /* Set tuning parameters for 8M and 150k clocks.
     * Note: this doesn't attempt to set the clocks to precise frequencies.
     * Instead, we calibrate these clocks against XTAL frequency later, when necessary.
     * - SCK_DCAP value controls tuning of 150k clock.
     *   The higher the value of DCAP is, the lower is the frequency.
     * - CK8M_DFREQ value controls tuning of 8M clock.
     *   CLK_8M_DFREQ constant gives the best temperature characteristics.
     */
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_SCK_DCAP, cfg.slow_clk_dcap);
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DFREQ, cfg.clk_8m_dfreq);

    /* Configure 8M clock division */
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL, cfg.clk_8m_div);

    /* Enable the internal bus used to configure PLLs */
    SET_PERI_REG_BITS(ANA_CONFIG_REG, ANA_CONFIG_M, ANA_CONFIG_M, ANA_CONFIG_S);
    CLEAR_PERI_REG_MASK(ANA_CONFIG_REG, I2C_APLL_M | I2C_BBPLL_M);

    /* Estimate XTAL frequency */
    rtc_xtal_freq_t xtal_freq = cfg.xtal_freq;
    if (xtal_freq == RTC_XTAL_FREQ_AUTO) {
        if (clk_val_is_valid(READ_PERI_REG(RTC_XTAL_FREQ_REG))) {
            /* XTAL frequency has already been set, use existing value */
            xtal_freq = rtc_clk_xtal_freq_get();
        } else {
            /* Not set yet, estimate XTAL frequency based on RTC_FAST_CLK */
            xtal_freq = rtc_clk_xtal_freq_estimate();
            if (xtal_freq == RTC_XTAL_FREQ_AUTO) {
                SOC_LOGW(TAG, "Can't estimate XTAL frequency, assuming 26MHz");
                xtal_freq = RTC_XTAL_FREQ_26M;
            }
        }
    } else if (!clk_val_is_valid(READ_PERI_REG(RTC_XTAL_FREQ_REG))) {
        /* Exact frequency was set in sdkconfig, but still warn if autodetected
         * frequency is different. If autodetection failed, worst case we get a
         * bit of garbage output.
         */

        rtc_xtal_freq_t est_xtal_freq = rtc_clk_xtal_freq_estimate();
        if (est_xtal_freq != xtal_freq) {
            SOC_LOGW(TAG, "Possibly invalid CONFIG_ESP32_XTAL_FREQ setting (%dMHz). Detected %d MHz.",
                    xtal_freq, est_xtal_freq);
        }
    }
    uart_tx_wait_idle(0);
    rtc_clk_xtal_freq_update(xtal_freq);
    rtc_clk_apb_freq_update(xtal_freq * MHZ);

    /* Set CPU frequency */

    rtc_clk_cpu_freq_get_config(&old_config);
    uint32_t freq_before = old_config.freq_mhz;

    bool res = rtc_clk_cpu_freq_mhz_to_config(cfg.cpu_freq_mhz, &new_config);
    if (!res) {
        SOC_LOGE(TAG, "invalid CPU frequency value");
        abort();
    }
    rtc_clk_cpu_freq_set_config(&new_config);

    /* Configure REF_TICK */
    REG_WRITE(APB_CTRL_XTAL_TICK_CONF_REG, xtal_freq - 1);
    REG_WRITE(APB_CTRL_PLL_TICK_CONF_REG, APB_CLK_FREQ / MHZ - 1); /* Under PLL, APB frequency is always 80MHz */

    /* Re-calculate the ccount to make time calculation correct. */
    XTHAL_SET_CCOUNT( (uint64_t)XTHAL_GET_CCOUNT() * cfg.cpu_freq_mhz / freq_before );

    /* Slow & fast clocks setup */
    if (cfg.slow_freq == RTC_SLOW_FREQ_32K_XTAL) {
        rtc_clk_32k_enable(true);
    }
    if (cfg.fast_freq == RTC_FAST_FREQ_8M) {
        bool need_8md256 = cfg.slow_freq == RTC_SLOW_FREQ_8MD256;
        rtc_clk_8m_enable(true, need_8md256);
    }
    rtc_clk_fast_freq_set(cfg.fast_freq);
    rtc_clk_slow_freq_set(cfg.slow_freq);
}

static rtc_xtal_freq_t rtc_clk_xtal_freq_estimate()
{
    /* Enable 8M/256 clock if needed */
    const bool clk_8m_enabled = rtc_clk_8m_enabled();
    const bool clk_8md256_enabled = rtc_clk_8md256_enabled();
    if (!clk_8md256_enabled) {
        rtc_clk_8m_enable(true, true);
    }

    uint64_t cal_val = rtc_clk_cal_ratio(RTC_CAL_8MD256, XTAL_FREQ_EST_CYCLES);
    /* cal_val contains period of 8M/256 clock in XTAL clock cycles
     * (shifted by RTC_CLK_CAL_FRACT bits).
     * Xtal frequency will be (cal_val * 8M / 256) / 2^19
     */
    uint32_t freq_mhz = (cal_val * RTC_FAST_CLK_FREQ_APPROX / MHZ / 256 ) >> RTC_CLK_CAL_FRACT;
    /* Guess the XTAL type. For now, only 40 and 26MHz are supported.
     */
    switch (freq_mhz) {
        case 21 ... 31:
            return RTC_XTAL_FREQ_26M;
        case 32 ... 33:
            SOC_LOGW(TAG, "Potentially bogus XTAL frequency: %d MHz, guessing 26 MHz", freq_mhz);
            return RTC_XTAL_FREQ_26M;
        case 34 ... 35:
            SOC_LOGW(TAG, "Potentially bogus XTAL frequency: %d MHz, guessing 40 MHz", freq_mhz);
            return RTC_XTAL_FREQ_40M;
        case 36 ... 45:
            return RTC_XTAL_FREQ_40M;
        default:
            SOC_LOGW(TAG, "Bogus XTAL frequency: %d MHz", freq_mhz);
            return RTC_XTAL_FREQ_AUTO;
    }
    /* Restore 8M and 8md256 clocks to original state */
    rtc_clk_8m_enable(clk_8m_enabled, clk_8md256_enabled);
}
