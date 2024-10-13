/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_rom_sys.h"
#include "hal/clk_tree_ll.h"
#include "hal/rtc_cntl_ll.h"
#include "hal/timer_ll.h"
#include "soc/rtc.h"
#include "soc/timer_periph.h"
#include "esp_hw_log.h"
#include "esp_private/periph_ctrl.h"

static const char *TAG = "rtc_time";

/* Calibration of RTC_SLOW_CLK is performed using a special feature of TIMG0.
 * This feature counts the number of XTAL clock cycles within a given number of
 * RTC_SLOW_CLK cycles.
 *
 * Slow clock calibration feature has two modes of operation: one-off and cycling.
 * In cycling mode (which is enabled by default on SoC reset), counting of XTAL
 * cycles within RTC_SLOW_CLK cycle is done continuously. Cycling mode is enabled
 * using TIMG_RTC_CALI_START_CYCLING bit. In one-off mode counting is performed
 * once, and TIMG_RTC_CALI_RDY bit is set when counting is done. One-off mode is
 * enabled using TIMG_RTC_CALI_START bit.
 */

/**
 * @brief Clock calibration function used by rtc_clk_cal and rtc_clk_cal_ratio
 * @param cal_clk which clock to calibrate
 * @param slowclk_cycles number of slow clock cycles to count. Max value is 32766.
 * @return number of XTAL clock cycles within the given number of slow clock cycles
 */
static uint32_t rtc_clk_cal_internal(rtc_cal_sel_t cal_clk, uint32_t slowclk_cycles)
{
    assert(slowclk_cycles < 32767);
    /* Enable requested clock (150k clock is always on) */
    bool dig_32k_xtal_enabled = clk_ll_xtal32k_digi_is_enabled();
    if (cal_clk == RTC_CAL_32K_XTAL && !dig_32k_xtal_enabled) {
        clk_ll_xtal32k_digi_enable();
    }

    bool rc_fast_enabled = clk_ll_rc_fast_is_enabled();
    bool rc_fast_d256_enabled = clk_ll_rc_fast_d256_is_enabled();
    if (cal_clk == RTC_CAL_8MD256) {
        rtc_clk_8m_enable(true, true);
        clk_ll_rc_fast_d256_digi_enable();
    }
    /* Prepare calibration */
    REG_SET_FIELD(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_CLK_SEL, cal_clk);
    CLEAR_PERI_REG_MASK(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_START_CYCLING);
    REG_SET_FIELD(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_MAX, slowclk_cycles);
    /* Figure out how long to wait for calibration to finish */
    uint32_t expected_freq;
    soc_rtc_slow_clk_src_t slow_clk_src = rtc_clk_slow_src_get();
    if (cal_clk == RTC_CAL_32K_XTAL ||
            (cal_clk == RTC_CAL_RTC_MUX && slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K)) {
        expected_freq = SOC_CLK_XTAL32K_FREQ_APPROX; /* standard 32k XTAL */
    } else if (cal_clk == RTC_CAL_8MD256 ||
               (cal_clk == RTC_CAL_RTC_MUX && slow_clk_src == SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256)) {
        expected_freq = SOC_CLK_RC_FAST_D256_FREQ_APPROX;
    } else {
        expected_freq = SOC_CLK_RC_SLOW_FREQ_APPROX; /* 150k internal oscillator */
    }
    uint32_t us_time_estimate = (uint32_t) (((uint64_t) slowclk_cycles) * MHZ / expected_freq);
    /* Check if the required number of slowclk_cycles may result in an overflow of TIMG_RTC_CALI_VALUE */
    soc_xtal_freq_t xtal_freq = rtc_clk_xtal_freq_get();
    if (xtal_freq == SOC_XTAL_FREQ_AUTO) {
        /* XTAL frequency is not known yet; assume worst case (40 MHz) */
        xtal_freq = SOC_XTAL_FREQ_40M;
    }
    const uint32_t us_timer_max =  TIMG_RTC_CALI_VALUE / (uint32_t) xtal_freq;
    if (us_time_estimate >= us_timer_max) {
        ESP_HW_LOGE(TAG, "slowclk_cycles value too large, possible overflow");
        return 0;
    }
    /* Start calibration */
    CLEAR_PERI_REG_MASK(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_START);
    SET_PERI_REG_MASK(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_START);
    /* Wait the expected time calibration should take.
     * TODO: if running under RTOS, and us_time_estimate > RTOS tick, use the
     * RTOS delay function.
     */
    esp_rom_delay_us(us_time_estimate);
    /* Wait for calibration to finish up to another us_time_estimate */
    int timeout_us = us_time_estimate;
    while (!GET_PERI_REG_MASK(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_RDY) &&
            timeout_us > 0) {
        timeout_us--;
        esp_rom_delay_us(1);
    }

    /* if dig_32k_xtal was originally off and enabled due to calibration, then set back to off state */
    if (cal_clk == RTC_CAL_32K_XTAL && !dig_32k_xtal_enabled) {
        clk_ll_xtal32k_digi_disable();
    }

    if (cal_clk == RTC_CAL_8MD256) {
        clk_ll_rc_fast_d256_digi_disable();
        rtc_clk_8m_enable(rc_fast_enabled, rc_fast_d256_enabled);
    }
    if (timeout_us == 0) {
        /* timed out waiting for calibration */
        return 0;
    }

    return REG_GET_FIELD(TIMG_RTCCALICFG1_REG(0), TIMG_RTC_CALI_VALUE);
}

uint32_t rtc_clk_cal_ratio(rtc_cal_sel_t cal_clk, uint32_t slowclk_cycles)
{
    assert(slowclk_cycles);
    uint64_t xtal_cycles = rtc_clk_cal_internal(cal_clk, slowclk_cycles);
    uint64_t ratio_64 = ((xtal_cycles << RTC_CLK_CAL_FRACT)) / slowclk_cycles;
    uint32_t ratio = (uint32_t)(ratio_64 & UINT32_MAX);
    return ratio;
}

static inline bool rtc_clk_cal_32k_valid(uint32_t xtal_freq, uint32_t slowclk_cycles, uint64_t actual_xtal_cycles)
{
    uint64_t expected_xtal_cycles = (xtal_freq * 1000000ULL * slowclk_cycles) >> 15; // xtal_freq(hz) * slowclk_cycles / 32768
    uint64_t delta = expected_xtal_cycles / 2000;                                    // 5/10000
    return (actual_xtal_cycles >= (expected_xtal_cycles - delta)) && (actual_xtal_cycles <= (expected_xtal_cycles + delta));
}

uint32_t rtc_clk_cal(rtc_cal_sel_t cal_clk, uint32_t slowclk_cycles)
{
    assert(slowclk_cycles);
    soc_xtal_freq_t xtal_freq = rtc_clk_xtal_freq_get();
    uint64_t xtal_cycles = rtc_clk_cal_internal(cal_clk, slowclk_cycles);

    if ((cal_clk == RTC_CAL_32K_XTAL) && !rtc_clk_cal_32k_valid((uint32_t)xtal_freq, slowclk_cycles, xtal_cycles)) {
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
    return rtc_cntl_ll_get_rtc_time();
}

void rtc_clk_wait_for_slow_cycle(void)
{
    REG_CLR_BIT(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_START_CYCLING | TIMG_RTC_CALI_START);
    REG_CLR_BIT(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_RDY);
    REG_SET_FIELD(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_CLK_SEL, RTC_CAL_RTC_MUX);
    /* Request to run calibration for 0 slow clock cycles.
     * RDY bit will be set on the nearest slow clock cycle.
     */
    REG_SET_FIELD(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_MAX, 0);
    REG_SET_BIT(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_START);
    esp_rom_delay_us(1); /* RDY needs some time to go low */
    int attempts = 1000;
    while (!GET_PERI_REG_MASK(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_RDY)) {
        esp_rom_delay_us(1);
        if (attempts) {
            if (--attempts == 0 && clk_ll_xtal32k_digi_is_enabled()) {
                ESP_HW_LOGE(TAG, "32kHz xtal has been stopped");
            }
        }
    }
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
    _timer_ll_enable_bus_clock(0, true);
    _timer_ll_reset_register(0);
#endif
}
