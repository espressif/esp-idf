/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/esp_clk_tree_common.h"
#include "hal/clk_tree_hal.h"
#include "hal/clk_tree_ll.h"
#include "soc/rtc.h"
#include "esp_private/esp_clk.h"
#include "soc/clk_tree_defs.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"
#include "esp_hw_log.h"

static const char *TAG = "esp_clk_tree_common";

typedef struct esp_clk_tree_calibrated_freq_t esp_clk_tree_calibrated_freq_t;

struct esp_clk_tree_calibrated_freq_t {
#if SOC_CLK_RC_FAST_D256_SUPPORTED
    uint32_t rc_fast_d256;
#elif SOC_CLK_RC_FAST_SUPPORT_CALIBRATION // && !SOC_CLK_RC_FAST_D256_SUPPORTED
    uint32_t rc_fast;
#endif
#if SOC_CLK_XTAL32K_SUPPORTED
    uint32_t xtal32k;
#endif
#if SOC_CLK_OSC_SLOW_SUPPORTED
    uint32_t osc_slow;
#endif
};

// TODO: Better to implement a spinlock for the static variables
static esp_clk_tree_calibrated_freq_t s_calibrated_freq = {};

/* Number of cycles for RTC_SLOW_CLK calibration */
#define RTC_SLOW_CLK_CAL_CYCLES     CONFIG_RTC_CLK_CAL_CYCLES
/* Number of cycles for ~32kHz clocks calibration (rc_fast_d256, xtal32k, osc_slow, rc32k) */
#define DEFAULT_32K_CLK_CAL_CYCLES  100
/* Number of cycles for RC_FAST calibration */
#define DEFAULT_RC_FAST_CAL_CYCLES  10000  // RC_FAST has a higher frequency, therefore, requires more cycles to get an accurate value


/**
 * Performs a frequency calibration to RTC slow clock
 *
 * slowclk_cycles Number of slow clock cycles to count.
 *                If slowclk_cycles = 0, calibration will not be performed. Clock's theoretical value will be used.
 *
 * Returns the number of XTAL clock cycles within the given number of slow clock cycles
 * It returns 0 if calibration failed, i.e. clock is not running
 */
static uint32_t clk_tree_rtc_slow_calibration(uint32_t slowclk_cycles)
{
    uint32_t cal_val = 0;
    if (slowclk_cycles > 0) {
        cal_val = rtc_clk_cal(RTC_CAL_RTC_MUX, slowclk_cycles);
    } else {
        const uint64_t cal_dividend = (1ULL << RTC_CLK_CAL_FRACT) * 1000000ULL;
        uint32_t source_approx_freq = clk_hal_lp_slow_get_freq_hz();
        assert(source_approx_freq);
        cal_val = (uint32_t)(cal_dividend / source_approx_freq);
    }
    if (cal_val) {
        ESP_EARLY_LOGD(TAG, "RTC_SLOW_CLK calibration value: %"PRIu32"", cal_val);
        // Update the calibration value of RTC_SLOW_CLK
        esp_clk_slowclk_cal_set(cal_val);
    }
    return cal_val;
}

#if SOC_CLK_RC_FAST_D256_SUPPORTED
uint32_t esp_clk_tree_rc_fast_d256_get_freq_hz(esp_clk_tree_src_freq_precision_t precision)
{
    switch (precision) {
    case ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX:
        return SOC_CLK_RC_FAST_D256_FREQ_APPROX;
    case ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED:
        if (!s_calibrated_freq.rc_fast_d256) {
            s_calibrated_freq.rc_fast_d256 = rtc_clk_freq_cal(rtc_clk_cal(RTC_CAL_8MD256, DEFAULT_32K_CLK_CAL_CYCLES));
        }
        return s_calibrated_freq.rc_fast_d256;
    case ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT:
        s_calibrated_freq.rc_fast_d256 = rtc_clk_freq_cal(rtc_clk_cal(RTC_CAL_8MD256, DEFAULT_32K_CLK_CAL_CYCLES));
        return s_calibrated_freq.rc_fast_d256;
    default:
        return 0;
    }
}
#endif

#if SOC_CLK_XTAL32K_SUPPORTED
uint32_t esp_clk_tree_xtal32k_get_freq_hz(esp_clk_tree_src_freq_precision_t precision)
{
    switch (precision) {
    case ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX:
        return SOC_CLK_XTAL32K_FREQ_APPROX;
    case ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED:
        if (!s_calibrated_freq.xtal32k) {
            s_calibrated_freq.xtal32k = rtc_clk_freq_cal(rtc_clk_cal(RTC_CAL_32K_XTAL, DEFAULT_32K_CLK_CAL_CYCLES));
        }
        return s_calibrated_freq.xtal32k;
    case ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT:
        s_calibrated_freq.xtal32k = rtc_clk_freq_cal(rtc_clk_cal(RTC_CAL_32K_XTAL, DEFAULT_32K_CLK_CAL_CYCLES));
        return s_calibrated_freq.xtal32k;
    default:
        return 0;
    }
}
#endif

#if SOC_CLK_OSC_SLOW_SUPPORTED
uint32_t esp_clk_tree_osc_slow_get_freq_hz(esp_clk_tree_src_freq_precision_t precision)
{
    switch (precision) {
    case ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX:
        return SOC_CLK_OSC_SLOW_FREQ_APPROX;
    case ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED:
        if (!s_calibrated_freq.osc_slow) {
            s_calibrated_freq.osc_slow = rtc_clk_freq_cal(rtc_clk_cal(RTC_CAL_32K_OSC_SLOW, DEFAULT_32K_CLK_CAL_CYCLES));
        }
        return s_calibrated_freq.osc_slow;
    case ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT:
        s_calibrated_freq.osc_slow = rtc_clk_freq_cal(rtc_clk_cal(RTC_CAL_32K_OSC_SLOW, DEFAULT_32K_CLK_CAL_CYCLES));
        return s_calibrated_freq.osc_slow;
    default:
        return 0;
    }
}
#endif

uint32_t esp_clk_tree_lp_slow_get_freq_hz(esp_clk_tree_src_freq_precision_t precision)
{
    switch (precision) {
    case ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED:
        // This returns calibrated (if CONFIG_xxx_RTC_CLK_CAL_CYCLES) value stored in RTC storage register
        return rtc_clk_freq_cal(clk_ll_rtc_slow_load_cal());
    case ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX:
        return clk_hal_lp_slow_get_freq_hz();
    case ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT:
        return rtc_clk_freq_cal(clk_tree_rtc_slow_calibration(RTC_SLOW_CLK_CAL_CYCLES));
    default:
        return 0;
    }
}

uint32_t esp_clk_tree_rc_fast_get_freq_hz(esp_clk_tree_src_freq_precision_t precision)
{
#if SOC_CLK_RC_FAST_SUPPORT_CALIBRATION
    if (precision == ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX) {
        return SOC_CLK_RC_FAST_FREQ_APPROX;
    }
#if SOC_CLK_RC_FAST_D256_SUPPORTED
    // If RC_FAST_D256 clock exists, calibration on a slow freq clock is much faster (less slow clock cycles need to wait)
    return esp_clk_tree_rc_fast_d256_get_freq_hz(precision) << 8;
#else
    // Calibrate directly on the RC_FAST clock requires much more slow clock cycles to get an accurate freq value
    if (precision != ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED || !s_calibrated_freq.rc_fast) {
        s_calibrated_freq.rc_fast = rtc_clk_freq_cal(rtc_clk_cal(RTC_CAL_RC_FAST, DEFAULT_RC_FAST_CAL_CYCLES));
    }
    return s_calibrated_freq.rc_fast;
#endif //SOC_CLK_RC_FAST_D256_SUPPORTED
#else //!SOC_CLK_RC_FAST_SUPPORT_CALIBRATION
    if (precision != ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX) {
        // No way of getting exact rc_fast freq
        ESP_HW_LOGW(TAG, "unable to get the exact freq of rc_fast_clk, returning its approx. freq value");
    }
    return SOC_CLK_RC_FAST_FREQ_APPROX;
#endif //SOC_CLK_RC_FAST_SUPPORT_CALIBRATION
}

uint32_t esp_clk_tree_lp_fast_get_freq_hz(esp_clk_tree_src_freq_precision_t precision)
{
    switch (clk_ll_rtc_fast_get_src()) {
    case SOC_RTC_FAST_CLK_SRC_XTAL_DIV:
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 //SOC_RTC_FAST_CLK_SRC_XTAL_D4
        return clk_hal_xtal_get_freq_mhz() * MHZ >> 2;
#else //SOC_RTC_FAST_CLK_SRC_XTAL_D2
        return clk_hal_xtal_get_freq_mhz() * MHZ >> 1;
#endif
    case SOC_RTC_FAST_CLK_SRC_RC_FAST:
        return esp_clk_tree_rc_fast_get_freq_hz(precision) / clk_ll_rc_fast_get_divider();
#if SOC_CLK_LP_FAST_SUPPORT_LP_PLL
    case SOC_RTC_FAST_CLK_SRC_LP_PLL:
        return clk_ll_lp_pll_get_freq_mhz() * MHZ;
#endif
    default:
        // Invalid clock source
        assert(false);
        return 0;
    }
}
