/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "esp32/rom/rtc.h"
#include "esp_rom_uart.h"
#include "esp_cpu.h"
#include "soc/rtc.h"
#include "soc/rtc_periph.h"
#include "hal/clk_tree_ll.h"
#include "hal/regi2c_ctrl_ll.h"
#include "esp_hw_log.h"
#include "sdkconfig.h"

/* Number of 8M/256 clock cycles to use for XTAL frequency estimation.
 * 10 cycles will take approximately 300 microseconds.
 */
#define XTAL_FREQ_EST_CYCLES            10

static rtc_xtal_freq_t rtc_clk_xtal_freq_estimate(void);
extern void rtc_clk_cpu_freq_to_xtal(int freq, int div);

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
    if (clk_ll_cpu_get_src() == SOC_CPU_CLK_SRC_PLL) {
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
    clk_ll_rc_fast_set_divider(cfg.clk_8m_div + 1);

    /* Reset (disable) i2c internal bus for all regi2c registers */
    regi2c_ctrl_ll_i2c_reset(); // TODO: This should be move out from rtc_clk_init
    /* Enable the internal bus used to configure PLLs */
    regi2c_ctrl_ll_i2c_bbpll_enable(); // TODO: This should be moved to bbpll_set_config
    regi2c_ctrl_ll_i2c_apll_enable(); // TODO: This should be moved to apll_set_config

    /* Estimate XTAL frequency */
    rtc_xtal_freq_t configured_xtal_freq = cfg.xtal_freq;
    rtc_xtal_freq_t stored_xtal_freq = rtc_clk_xtal_freq_get(); // read the xtal freq value from RTC storage register
    rtc_xtal_freq_t xtal_freq = configured_xtal_freq;
    if (configured_xtal_freq == RTC_XTAL_FREQ_AUTO) {
        if (stored_xtal_freq != RTC_XTAL_FREQ_AUTO) {
            /* XTAL frequency has already been set, use existing value */
            xtal_freq = stored_xtal_freq;
        } else {
            /* Not set yet, estimate XTAL frequency based on RTC_FAST_CLK */
            xtal_freq = rtc_clk_xtal_freq_estimate();
            if (xtal_freq == RTC_XTAL_FREQ_AUTO) {
                ESP_HW_LOGW(TAG, "Can't estimate XTAL frequency, assuming 26MHz");
                xtal_freq = RTC_XTAL_FREQ_26M;
            }
        }
    } else if (stored_xtal_freq == RTC_XTAL_FREQ_AUTO) {
        /* Exact frequency was set in sdkconfig, but still warn if autodetected
         * frequency is different. If autodetection failed, worst case we get a
         * bit of garbage output.
         */

        rtc_xtal_freq_t est_xtal_freq = rtc_clk_xtal_freq_estimate();
        if (est_xtal_freq != configured_xtal_freq) {
            ESP_HW_LOGW(TAG, "Possibly invalid CONFIG_XTAL_FREQ setting (%dMHz). Detected %d MHz.",
                    configured_xtal_freq, est_xtal_freq);
        }
    }
    esp_rom_uart_tx_wait_idle(0);
    rtc_clk_xtal_freq_update(xtal_freq);
    rtc_clk_apb_freq_update(xtal_freq * MHZ);

    /* Set CPU frequency */

    rtc_clk_cpu_freq_get_config(&old_config);
    uint32_t freq_before = old_config.freq_mhz;

    bool res = rtc_clk_cpu_freq_mhz_to_config(cfg.cpu_freq_mhz, &new_config);
    if (!res) {
        ESP_HW_LOGE(TAG, "invalid CPU frequency value");
        abort();
    }
    rtc_clk_cpu_freq_set_config(&new_config);

    /* Configure REF_TICK */
    // Initialize it in the bootloader stage, but it is not a must to do here
    // REF_TICK divider value always gets updated when switching cpu clock source
    clk_ll_ref_tick_set_divider(SOC_CPU_CLK_SRC_XTAL, xtal_freq);
    clk_ll_ref_tick_set_divider(SOC_CPU_CLK_SRC_PLL, new_config.freq_mhz);

    /* Re-calculate the ccount to make time calculation correct. */
    esp_cpu_set_cycle_count( (uint64_t)esp_cpu_get_cycle_count() * cfg.cpu_freq_mhz / freq_before );

    /* Slow & fast clocks setup */
    // We will not power off RC_FAST in bootloader stage even if it is not being used as any
    // cpu / rtc_fast / rtc_slow clock sources, this is because RNG always needs it in the bootloader stage.
    bool need_rc_fast_en = true;
    bool need_rc_fast_d256_en = false;
    if (cfg.slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
        rtc_clk_32k_enable(true);
    } else if (cfg.slow_clk_src == SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256) {
        need_rc_fast_d256_en = true;
    }
    rtc_clk_8m_enable(need_rc_fast_en, need_rc_fast_d256_en);
    rtc_clk_fast_src_set(cfg.fast_clk_src);
    rtc_clk_slow_src_set(cfg.slow_clk_src);
}

static rtc_xtal_freq_t rtc_clk_xtal_freq_estimate(void)
{
#if CONFIG_IDF_ENV_FPGA
    return RTC_XTAL_FREQ_40M;
#endif // CONFIG_IDF_ENV_FPGA
    rtc_xtal_freq_t xtal_freq;
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
    uint32_t freq_mhz = (cal_val * SOC_CLK_RC_FAST_FREQ_APPROX / MHZ / 256 ) >> RTC_CLK_CAL_FRACT;
    /* Guess the XTAL type. For now, only 40 and 26MHz are supported.
     */
    switch (freq_mhz) {
    case 21 ... 31:
        xtal_freq = RTC_XTAL_FREQ_26M;
        break;
    case 32 ... 33:
        ESP_HW_LOGW(TAG, "Potentially bogus XTAL frequency: %"PRIu32" MHz, guessing 26 MHz", freq_mhz);
        xtal_freq = RTC_XTAL_FREQ_26M;
        break;
    case 34 ... 35:
        ESP_HW_LOGW(TAG, "Potentially bogus XTAL frequency: %"PRIu32" MHz, guessing 40 MHz", freq_mhz);
        xtal_freq = RTC_XTAL_FREQ_40M;
        break;
    case 36 ... 45:
        xtal_freq = RTC_XTAL_FREQ_40M;
        break;
    default:
        ESP_HW_LOGW(TAG, "Bogus XTAL frequency: %"PRIu32" MHz", freq_mhz);
        xtal_freq = RTC_XTAL_FREQ_AUTO;
        break;
    }
    /* Restore 8M and 8md256 clocks to original state */
    rtc_clk_8m_enable(clk_8m_enabled, clk_8md256_enabled);
    return xtal_freq;
}
