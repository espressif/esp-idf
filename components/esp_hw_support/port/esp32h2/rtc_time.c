/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp32h2/rom/ets_sys.h"
#include "soc/rtc.h"
#include "soc/lp_timer_reg.h"
#include "hal/clk_tree_ll.h"
#include "hal/timer_ll.h"
#include "soc/timer_group_reg.h"
#include "esp_rom_sys.h"
#include "assert.h"
#include "hal/efuse_hal.h"
#include "soc/chip_revision.h"
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

/* On ESP32H2, TIMG_RTC_CALI_CLK_SEL can config to 0, 1, 2, 3
 * 0 or 3: calibrate RC_SLOW clock
 * 1: calibrate RC_FAST clock
 * 2: calibrate 32K clock, which 32k depends on reg_32k_sel: 0: Internal 32 kHz RC oscillator, 1: External 32 kHz XTAL, 2: External 32kHz clock input by gpio13
 */
#define TIMG_RTC_CALI_CLK_SEL_RC_SLOW 0
#define TIMG_RTC_CALI_CLK_SEL_RC_FAST 1
#define TIMG_RTC_CALI_CLK_SEL_32K     2

/**
 * @brief Clock calibration function used by rtc_clk_cal
 *
 * Calibration of RTC_SLOW_CLK is performed using a special feature of TIMG0.
 * This feature counts the number of XTAL clock cycles within a given number of
 * RTC_SLOW_CLK cycles.
 *
 * Slow clock calibration feature has two modes of operation: one-off and cycling.
 * In cycling mode (which is enabled by default on SoC reset), counting of XTAL
 * cycles within RTC_SLOW_CLK cycle is done continuously. Cycling mode is enabled
 * using TIMG_RTC_CALI_START_CYCLING bit. In one-off mode counting is performed
 * once, and TIMG_RTC_CALI_RDY bit is set when counting is done. One-off mode is
 * enabled using TIMG_RTC_CALI_START bit.
 *
 * @param cal_clk which clock to calibrate
 * @param slowclk_cycles number of slow clock cycles to count
 * @return number of XTAL clock cycles within the given number of slow clock cycles
 */
static uint32_t rtc_clk_cal_internal(rtc_cal_sel_t cal_clk, uint32_t slowclk_cycles)
{
    assert(slowclk_cycles < TIMG_RTC_CALI_MAX_V);

    uint32_t cali_clk_sel = 0;
    soc_rtc_slow_clk_src_t slow_clk_src = rtc_clk_slow_src_get();
    soc_rtc_slow_clk_src_t old_32k_cal_clk_sel = clk_ll_32k_calibration_get_target();
    if (cal_clk == RTC_CAL_RTC_MUX) {
        cal_clk = (rtc_cal_sel_t)slow_clk_src;
    }
    if (cal_clk == RTC_CAL_RC_FAST) {
        cali_clk_sel = TIMG_RTC_CALI_CLK_SEL_RC_FAST;
    } else if (cal_clk == RTC_CAL_RC_SLOW) {
        cali_clk_sel = TIMG_RTC_CALI_CLK_SEL_RC_SLOW;
    } else {
        cali_clk_sel = TIMG_RTC_CALI_CLK_SEL_32K;
        clk_ll_32k_calibration_set_target((soc_rtc_slow_clk_src_t)cal_clk);
    }


    /* Enable requested clock (150k clock is always on) */
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
    REG_SET_FIELD(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_CLK_SEL, cali_clk_sel);
    CLEAR_PERI_REG_MASK(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_START_CYCLING);
    REG_SET_FIELD(TIMG_RTCCALICFG_REG(0), TIMG_RTC_CALI_MAX, slowclk_cycles);
    /* Figure out how long to wait for calibration to finish */

    /* Set timeout reg and expect time delay*/
    uint32_t expected_freq;
    if (cali_clk_sel == TIMG_RTC_CALI_CLK_SEL_32K) {
        REG_SET_FIELD(TIMG_RTCCALICFG2_REG(0), TIMG_RTC_CALI_TIMEOUT_THRES, RTC_SLOW_CLK_32K_CAL_TIMEOUT_THRES(slowclk_cycles));
        expected_freq = SOC_CLK_XTAL32K_FREQ_APPROX;
    } else if (cali_clk_sel == TIMG_RTC_CALI_CLK_SEL_RC_FAST) {
        REG_SET_FIELD(TIMG_RTCCALICFG2_REG(0), TIMG_RTC_CALI_TIMEOUT_THRES, RTC_FAST_CLK_8M_CAL_TIMEOUT_THRES(slowclk_cycles));
        expected_freq = SOC_CLK_RC_FAST_FREQ_APPROX;
    } else {
        REG_SET_FIELD(TIMG_RTCCALICFG2_REG(0), TIMG_RTC_CALI_TIMEOUT_THRES, RTC_SLOW_CLK_150K_CAL_TIMEOUT_THRES(slowclk_cycles));
        expected_freq = SOC_CLK_RC_SLOW_FREQ_APPROX;
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

            /*The Fosc CLK of calibration circuit is divided by 32 for ECO2.
              So we need to multiply the frequency of the Fosc for ECO2 and above chips by 32 times.
              And ensure that this modification will not affect ECO0 and ECO1.*/
            if (ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 2)) {
                if (cal_clk == RTC_CAL_RC_FAST) {
                    cal_val = cal_val >> 5;
                }
            }
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

    // Always set back the calibration 32kHz clock selection
    if (old_32k_cal_clk_sel != SOC_RTC_SLOW_CLK_SRC_INVALID) {
        clk_ll_32k_calibration_set_target(old_32k_cal_clk_sel);
    }

    return cal_val;
}

static bool rtc_clk_cal_32k_valid(rtc_xtal_freq_t xtal_freq, uint32_t slowclk_cycles, uint64_t actual_xtal_cycles)
{
    uint64_t expected_xtal_cycles = (xtal_freq * 1000000ULL * slowclk_cycles) >> 15; // xtal_freq(hz) * slowclk_cycles / 32768
    uint64_t delta = expected_xtal_cycles / 2000;                                    // 5/10000 = 0.05% error range
    return (actual_xtal_cycles >= (expected_xtal_cycles - delta)) && (actual_xtal_cycles <= (expected_xtal_cycles + delta));
}

uint32_t rtc_clk_cal(rtc_cal_sel_t cal_clk, uint32_t slowclk_cycles)
{
    assert(slowclk_cycles);
    rtc_xtal_freq_t xtal_freq = rtc_clk_xtal_freq_get();

    /*The Fosc CLK of calibration circuit is divided by 32 for ECO2.
      So we need to divide the calibrate cycles of the FOSC for ECO1 and above chips by 32 to
      avoid excessive calibration time.*/
    if (ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 2)) {
        if (cal_clk == RTC_CAL_RC_FAST) {
            slowclk_cycles = slowclk_cycles >> 5;
        }
    }

    uint64_t xtal_cycles = rtc_clk_cal_internal(cal_clk, slowclk_cycles);

    if (cal_clk == RTC_CAL_32K_XTAL && !rtc_clk_cal_32k_valid(xtal_freq, slowclk_cycles, xtal_cycles)) {
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
    SET_PERI_REG_MASK(LP_TIMER_UPDATE_REG, LP_TIMER_MAIN_TIMER_UPDATE);
    uint64_t t = READ_PERI_REG(LP_TIMER_MAIN_BUF0_LOW_REG);
    t |= ((uint64_t) READ_PERI_REG(LP_TIMER_MAIN_BUF0_HIGH_REG)) << 32;
    return t;
}

void rtc_clk_wait_for_slow_cycle(void) //This function may not by useful any more
{
    // TODO: IDF-6254
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
    PERIPH_RCC_ACQUIRE_ATOMIC(PERIPH_TIMG0_MODULE, ref_count) {
        if (ref_count == 0) {
            timer_ll_enable_bus_clock(0, true);
            timer_ll_reset_register(0);
        }
    }
}
