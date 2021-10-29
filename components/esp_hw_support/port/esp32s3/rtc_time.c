/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_rom_sys.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/timer_group_reg.h"

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
 * @param slowclk_cycles number of slow clock cycles to count
 * @return number of XTAL clock cycles within the given number of slow clock cycles
 */
uint32_t rtc_clk_cal_internal(rtc_cal_sel_t cal_clk, uint32_t slowclk_cycles)
{
    /* On ESP32S3, choosing RTC_CAL_RTC_MUX results in calibration of
     * the 150k RTC clock regardless of the currenlty selected SLOW_CLK.
     * On the ESP32, it used the currently selected SLOW_CLK.
     * The following code emulates ESP32 behavior:
     */
    if (cal_clk == RTC_CAL_RTC_MUX) {
        rtc_slow_freq_t slow_freq = rtc_clk_slow_freq_get();
        if (slow_freq == RTC_SLOW_FREQ_32K_XTAL) {
            cal_clk = RTC_CAL_32K_XTAL;
        } else if (slow_freq == RTC_SLOW_FREQ_8MD256) {
            cal_clk = RTC_CAL_8MD256;
        }
    } else if (cal_clk == RTC_CAL_INTERNAL_OSC) {
        cal_clk = RTC_CAL_RTC_MUX;
    }

    /* Enable requested clock (150k clock is always on) */
    int dig_32k_xtal_state = REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_XTAL32K_EN);
    if (cal_clk == RTC_CAL_32K_XTAL && !dig_32k_xtal_state) {
        REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_XTAL32K_EN, 1);
    }

    if (cal_clk == RTC_CAL_8MD256) {
        SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_D256_EN);
    }
    /* There may be another calibration process already running during we call this function,
     * so we should wait the last process is done.
     */
    if (GET_PERI_REG_MASK(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_START_CYCLING)) {
        while (!GET_PERI_REG_MASK(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_RDY)
               && !GET_PERI_REG_MASK(TIMG_RTCCALICFG2_REG(0), TIMG_RTC_CALI_TIMEOUT));
    }

    /* Prepare calibration */
    REG_SET_FIELD(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_CLK_SEL, cal_clk);
    CLEAR_PERI_REG_MASK(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_START_CYCLING);
    REG_SET_FIELD(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_MAX, slowclk_cycles);
    /* Figure out how long to wait for calibration to finish */

    /* Set timeout reg and expect time delay*/
    uint32_t expected_freq;
    if (cal_clk == RTC_CAL_32K_XTAL) {
        REG_SET_FIELD(TIMG_RTCCALICFG2_REG(0), TIMG_RTC_CALI_TIMEOUT_THRES, RTC_SLOW_CLK_X32K_CAL_TIMEOUT_THRES(slowclk_cycles));
        expected_freq = RTC_SLOW_CLK_FREQ_32K;
    } else if (cal_clk == RTC_CAL_8MD256) {
        REG_SET_FIELD(TIMG_RTCCALICFG2_REG(0), TIMG_RTC_CALI_TIMEOUT_THRES, RTC_SLOW_CLK_8MD256_CAL_TIMEOUT_THRES(slowclk_cycles));
        expected_freq = RTC_SLOW_CLK_FREQ_8MD256;
    } else {
        REG_SET_FIELD(TIMG_RTCCALICFG2_REG(0), TIMG_RTC_CALI_TIMEOUT_THRES, RTC_SLOW_CLK_150K_CAL_TIMEOUT_THRES(slowclk_cycles));
        expected_freq = RTC_SLOW_CLK_FREQ_150K;
    }
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
            break;
        }
        if (GET_PERI_REG_MASK(TIMG_RTCCALICFG2_REG(0), TIMG_RTC_CALI_TIMEOUT)) {
            cal_val = 0;
            break;
        }
    }
    CLEAR_PERI_REG_MASK(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_START);

    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_XTAL32K_EN, dig_32k_xtal_state);

    if (cal_clk == RTC_CAL_8MD256) {
        CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_D256_EN);
    }

    return cal_val;
}

uint32_t rtc_clk_cal_ratio(rtc_cal_sel_t cal_clk, uint32_t slowclk_cycles)
{
    uint64_t xtal_cycles = rtc_clk_cal_internal(cal_clk, slowclk_cycles);
    uint64_t ratio_64 = ((xtal_cycles << RTC_CLK_CAL_FRACT)) / slowclk_cycles;
    uint32_t ratio = (uint32_t)(ratio_64 & UINT32_MAX);
    return ratio;
}

uint32_t rtc_clk_cal(rtc_cal_sel_t cal_clk, uint32_t slowclk_cycles)
{
    rtc_xtal_freq_t xtal_freq = rtc_clk_xtal_freq_get();
    uint64_t xtal_cycles = rtc_clk_cal_internal(cal_clk, slowclk_cycles);
    uint64_t divider = ((uint64_t)xtal_freq) * slowclk_cycles;
    uint64_t period_64 = ((xtal_cycles << RTC_CLK_CAL_FRACT) + divider / 2 - 1) / divider;
    uint32_t period = (uint32_t)(period_64 & UINT32_MAX);
    return period;
}

uint64_t rtc_time_us_to_slowclk(uint64_t time_in_us, uint32_t period)
{
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
    SET_PERI_REG_MASK(RTC_CNTL_TIME_UPDATE_REG, RTC_CNTL_TIME_UPDATE);
    uint64_t t = READ_PERI_REG(RTC_CNTL_TIME0_REG);
    t |= ((uint64_t) READ_PERI_REG(RTC_CNTL_TIME1_REG)) << 32;
    return t;
}

uint64_t rtc_light_slp_time_get(void)
{
    uint64_t t_wake = READ_PERI_REG(RTC_CNTL_TIME_LOW0_REG);
    t_wake |= ((uint64_t) READ_PERI_REG(RTC_CNTL_TIME_HIGH0_REG)) << 32;
    uint64_t t_slp = READ_PERI_REG(RTC_CNTL_TIME_LOW1_REG);
    t_slp |= ((uint64_t) READ_PERI_REG(RTC_CNTL_TIME_HIGH1_REG)) << 32;
    return (t_wake - t_slp);
}

uint64_t rtc_deep_slp_time_get(void)
{
    uint64_t t_slp = READ_PERI_REG(RTC_CNTL_TIME_LOW1_REG);
    t_slp |= ((uint64_t) READ_PERI_REG(RTC_CNTL_TIME_HIGH1_REG)) << 32;
    uint64_t t_wake = rtc_time_get();
    return (t_wake - t_slp);
}

void rtc_clk_wait_for_slow_cycle(void) //This function may not by useful any more
{
    SET_PERI_REG_MASK(RTC_CNTL_SLOW_CLK_CONF_REG, RTC_CNTL_SLOW_CLK_NEXT_EDGE);
    while (GET_PERI_REG_MASK(RTC_CNTL_SLOW_CLK_CONF_REG, RTC_CNTL_SLOW_CLK_NEXT_EDGE)) {
        esp_rom_delay_us(1);
    }
}

uint32_t rtc_clk_freq_cal(uint32_t cal_val)
{
    if (cal_val == 0) {
        return 0;   // cal_val will be denominator, return 0 as the symbol of failure.
    }
    return 1000000ULL * (1 << RTC_CLK_CAL_FRACT) / cal_val;
}
