/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <assert.h>
#include "esp32p4/rom/ets_sys.h"
#include "soc/rtc.h"
#include "hal/lp_timer_hal.h"
#include "hal/clk_tree_ll.h"
#include "hal/timer_ll.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "soc/timer_group_reg.h"
#include "esp_rom_sys.h"
#include "esp_private/periph_ctrl.h"

static const char *TAG = "rtc_time";

/* Calibration of clock frequency is performed using a special feature of TIMG0.
 * This feature counts the number of XTAL clock cycles within a given number of
 * clock cycles.
 */

#define CLK_CAL_TIMEOUT_THRES(cal_clk, cycles) ((cal_clk == RTC_CAL_RC32K || cal_clk == RTC_CAL_32K_XTAL) ? (cycles << 12) : (cycles << 10))

// Calibration can only be performed on relatively slow speed clock signal. Therefore, for high-speed clocks,
// calibration is performed on their DIV_CLKs. The divider is configurable. We set:
#define CLK_CAL_DIV_VAL(cal_clk) \
            ((cal_clk == RTC_CAL_RC_SLOW || cal_clk == RTC_CAL_RC32K || cal_clk == RTC_CAL_32K_XTAL) ? 1 : \
             (cal_clk == RTC_CAL_LP_PLL) ? 50 : \
             (cal_clk == RTC_CAL_RC_FAST) ? 200 : \
             4000)

// CLK_CAL_FREQ_APPROX = CLK_FREQ_APPROX / CLK_CAL_DIV_VAL
#define CLK_CAL_FREQ_APPROX(cal_clk) \
            ((cal_clk == RTC_CAL_MPLL) ? (CLK_LL_PLL_500M_FREQ_MHZ * MHZ / 4000) : \
             (cal_clk == RTC_CAL_SPLL) ? (CLK_LL_PLL_480M_FREQ_MHZ * MHZ / 4000) : \
             (cal_clk == RTC_CAL_CPLL) ? (CLK_LL_PLL_400M_FREQ_MHZ * MHZ / 4000) : \
             (cal_clk == RTC_CAL_APLL) ? (105 * MHZ / 4000) : \
             (cal_clk == RTC_CAL_SDIO_PLL0 || cal_clk == RTC_CAL_SDIO_PLL1 || cal_clk == RTC_CAL_SDIO_PLL2) ? (200 * MHZ / 4000) : \
             (cal_clk == RTC_CAL_RC_FAST) ? (SOC_CLK_RC_FAST_FREQ_APPROX / 200) : \
             (cal_clk == RTC_CAL_RC_SLOW) ? (SOC_CLK_RC_SLOW_FREQ_APPROX) : \
             (cal_clk == RTC_CAL_RC32K) ? (SOC_CLK_RC32K_FREQ_APPROX) : \
             (cal_clk == RTC_CAL_32K_XTAL) ? (SOC_CLK_XTAL32K_FREQ_APPROX) : \
             (cal_clk == RTC_CAL_LP_PLL) ? (CLK_LL_PLL_8M_FREQ_MHZ * MHZ / 50) : \
             0)

uint32_t rtc_clk_cal_internal(rtc_cal_sel_t cal_clk, uint32_t slowclk_cycles)
{
    assert(slowclk_cycles < TIMG_RTC_CALI_MAX_V);

    if (cal_clk == RTC_CAL_RTC_MUX) {
        soc_rtc_slow_clk_src_t slow_clk_src = rtc_clk_slow_src_get();
        if (slow_clk_src == SOC_RTC_SLOW_CLK_SRC_RC_SLOW) {
            cal_clk = RTC_CAL_RC_SLOW;
        } else if (slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
            cal_clk = RTC_CAL_32K_XTAL;
        } else if (slow_clk_src == SOC_RTC_SLOW_CLK_SRC_RC32K) {
            cal_clk = RTC_CAL_RC32K;
        }
    }
    if (cal_clk < 0 || cal_clk >= RTC_CAL_INVALID_CLK) {
        ESP_EARLY_LOGE(TAG, "clock not supported to be calibrated");
        return 0;
    }

    /* Enable requested clock (some clocks are always on) */
    // All clocks on/off takes time to be stable, so we shouldn't frequently enable/disable the clock
    // Only enable if orignally was disabled, and set back to the disable state after calibration is done
    // If the clock is already on, then do nothing
    bool dig_32k_xtal_enabled = clk_ll_xtal32k_digi_is_enabled();
    if (cal_clk == RTC_CAL_32K_XTAL && !dig_32k_xtal_enabled) {
            clk_ll_xtal32k_digi_enable();
    }

    bool rc_fast_enabled = clk_ll_rc_fast_is_enabled();
    bool dig_rc_fast_enabled = clk_ll_rc_fast_digi_is_enabled();
    if (cal_clk == RTC_CAL_RC_FAST) {
        if (!rc_fast_enabled) {
            rtc_clk_8m_enable(true);
        }
        if (!dig_rc_fast_enabled) {
            rtc_dig_clk8m_enable();
        }
    }

    bool rc32k_enabled = clk_ll_rc32k_is_enabled();
    bool dig_rc32k_enabled = clk_ll_rc32k_digi_is_enabled();
    if (cal_clk == RTC_CAL_RC32K) {
        if (!rc32k_enabled) {
            rtc_clk_rc32k_enable(true);
        }
        if (!dig_rc32k_enabled) {
            clk_ll_rc32k_digi_enable();
        }
    }

    /* There may be another calibration process already running during we call this function,
     * so we should wait the last process is done.
     */
    if (GET_PERI_REG_MASK(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_START_CYCLING)) {
        /**
         * Set a small timeout threshold to accelerate the generation of timeout.
         * The internal circuit will be reset when the timeout occurs and will not affect the next calibration.
         */
        REG_SET_FIELD(TIMG_RTCCALICFG2_REG(0), TIMG_RTC_CALI_TIMEOUT_THRES, 1);
        while (!GET_PERI_REG_MASK(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_RDY)
               && !GET_PERI_REG_MASK(TIMG_RTCCALICFG2_REG(0), TIMG_RTC_CALI_TIMEOUT));
    }

    /* Prepare calibration */
    REG_SET_FIELD(HP_SYS_CLKRST_PERI_CLK_CTRL21_REG, HP_SYS_CLKRST_REG_TIMERGRP0_TGRT_CLK_SRC_SEL, cal_clk);
    uint32_t clk_cal_divider = CLK_CAL_DIV_VAL(cal_clk);
    REG_SET_FIELD(HP_SYS_CLKRST_PERI_CLK_CTRL21_REG, HP_SYS_CLKRST_REG_TIMERGRP0_TGRT_CLK_DIV_NUM, clk_cal_divider - 1);
    CLEAR_PERI_REG_MASK(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_START_CYCLING);
    REG_SET_FIELD(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_MAX, slowclk_cycles);
    /* Figure out how long to wait for calibration to finish */

    /* Set timeout reg and expect time delay*/
    REG_SET_FIELD(TIMG_RTCCALICFG2_REG(0), TIMG_RTC_CALI_TIMEOUT_THRES, CLK_CAL_TIMEOUT_THRES(cal_clk, slowclk_cycles));
    uint32_t expected_freq = CLK_CAL_FREQ_APPROX(cal_clk);
    assert(expected_freq);
    uint32_t us_time_estimate = (uint32_t) (((uint64_t) slowclk_cycles) * MHZ / expected_freq);
    /* Start calibration */
    CLEAR_PERI_REG_MASK(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_START);
    SET_PERI_REG_MASK(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_START);

    /* Wait for calibration to finish up to another us_time_estimate */
    esp_rom_delay_us(us_time_estimate);
    uint32_t cal_val;
    while (true) {
        if (GET_PERI_REG_MASK(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_RDY)) {
            cal_val = REG_GET_FIELD(TIMG_RTCCALICFG1_REG(0), TIMG_RTC_CALI_VALUE);
            cal_val /= clk_cal_divider;
            break;
        }
        if (GET_PERI_REG_MASK(TIMG_RTCCALICFG2_REG(0), TIMG_RTC_CALI_TIMEOUT)) {
            cal_val = 0;
            break;
        }
    }
    CLEAR_PERI_REG_MASK(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_START);

    /* if dig_32k_xtal was originally off and enabled due to calibration, then set back to off state */
    if (cal_clk == RTC_CAL_32K_XTAL && !dig_32k_xtal_enabled) {
        clk_ll_xtal32k_digi_disable();
    }

    if (cal_clk == RTC_CAL_RC_FAST) {
        if (!dig_rc_fast_enabled) {
            rtc_dig_clk8m_disable();
        }
        if (!rc_fast_enabled) {
            rtc_clk_8m_enable(false);
        }
    }

    if (cal_clk == RTC_CAL_RC32K) {
        if (!dig_rc32k_enabled) {
            clk_ll_rc32k_digi_disable();
        }
        if (!rc32k_enabled) {
            rtc_clk_rc32k_enable(false);
        }
    }

    return cal_val;
}

static bool rtc_clk_cal_32k_valid(uint32_t xtal_freq, uint32_t slowclk_cycles, uint64_t actual_xtal_cycles)
{
    uint64_t expected_xtal_cycles = (xtal_freq * 1000000ULL * slowclk_cycles) >> 15; // xtal_freq(hz) * slowclk_cycles / 32768
    uint64_t delta = expected_xtal_cycles / 2000;                                    // 5/10000 = 0.05% error range
    return (actual_xtal_cycles >= (expected_xtal_cycles - delta)) && (actual_xtal_cycles <= (expected_xtal_cycles + delta));
}

uint32_t rtc_clk_cal(rtc_cal_sel_t cal_clk, uint32_t slowclk_cycles)
{
    assert(slowclk_cycles);
    soc_xtal_freq_t xtal_freq = rtc_clk_xtal_freq_get();
    uint64_t xtal_cycles = rtc_clk_cal_internal(cal_clk, slowclk_cycles);

    if (cal_clk == RTC_CAL_32K_XTAL && !rtc_clk_cal_32k_valid((uint32_t)xtal_freq, slowclk_cycles, xtal_cycles)) {
        return 0;
    }

    uint64_t divider = ((uint64_t)xtal_freq) * slowclk_cycles;
    uint64_t period_64 = ((xtal_cycles << RTC_CLK_CAL_FRACT) + divider / 2 - 1) / divider;
    uint32_t period = (uint32_t)(period_64 & UINT32_MAX);
    return period;
}

uint64_t rtc_time_us_to_slowclk(uint64_t time_in_us, uint32_t period)
{
    assert(period);
    /* Overflow will happen in this function if time_in_us >= 2^45, which is about 400 days.
     * TODO: fix overflow.
     */
    return (time_in_us << RTC_CLK_CAL_FRACT) / period;
}

uint64_t rtc_time_slowclk_to_us(uint64_t rtc_cycles, uint32_t period)
{
    return (rtc_cycles * period) >> RTC_CLK_CAL_FRACT;
}

uint64_t rtc_time_get(void)
{
    return lp_timer_hal_get_cycle_count();
}

void rtc_clk_wait_for_slow_cycle(void) //This function may not by useful any more
{
    // TODO: IDF-5781
    ESP_EARLY_LOGW(TAG, "rtc_clk_wait_for_slow_cycle() has not been implemented yet");
}

uint32_t rtc_clk_freq_cal(uint32_t cal_val)
{
    if (cal_val == 0) {
        return 0;   // cal_val will be denominator, return 0 as the symbol of failure.
    }
    return 1000000ULL * (1 << RTC_CLK_CAL_FRACT) / cal_val;
}

/// @brief if the calibration is used, we need to enable the timer group0 first
__attribute__((constructor))
static void enable_timer_group0_for_calibration(void)
{
#ifndef BOOTLOADER_BUILD
    PERIPH_RCC_ACQUIRE_ATOMIC(PERIPH_TIMG0_MODULE, ref_count) {
        if (ref_count == 0) {
            timer_ll_enable_bus_clock(0, true);
            timer_ll_reset_register(0);
        }
    }
#else
    // no critical section is needed for bootloader
    int __DECLARE_RCC_RC_ATOMIC_ENV;
    timer_ll_enable_bus_clock(0, true);
    timer_ll_reset_register(0);
#endif
}
