/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/esp_clk_tree_derived.h"
#include "esp_private/critical_section.h"
#include "esp_private/periph_ctrl.h"
#include "hal/clk_tree_hal.h"
#include "hal/clk_tree_ll.h"
#include "soc/rtc.h"
#include "esp_private/esp_clk.h"
#include "soc/clk_tree_defs.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"
#include "esp_check.h"
#include "esp_hw_log.h"
#include "esp_log.h"
#if SOC_CLK_MPLL_SUPPORTED
#include "esp_private/rtc_clk.h"
#include "esp_ldo_regulator.h"
#endif

ESP_HW_LOG_ATTR_TAG(TAG, "esp_clk_tree_common");

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
                                           // Usually we calibrate on the divider of the RC_FAST clock, the cal_cycles is divided by
                                           // the divider factor internally in rtc_clk_cal, so the time to spend on calibrating RC_FAST
                                           // is always (10000 / f_rc_fast)


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
        cal_val = rtc_clk_cal(CLK_CAL_RTC_SLOW, slowclk_cycles);
    } else {
        const uint64_t cal_dividend = (1ULL << RTC_CLK_CAL_FRACT) * 1000000ULL;
        uint32_t source_approx_freq = clk_hal_lp_slow_get_freq_hz();
        assert(source_approx_freq);
        cal_val = (uint32_t)(cal_dividend / source_approx_freq);
    }
    if (cal_val) {
        ESP_EARLY_LOGD(TAG, "RTC_SLOW_CLK calibration value: %" PRIu32, cal_val);
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
            s_calibrated_freq.rc_fast_d256 = rtc_clk_freq_cal(rtc_clk_cal(CLK_CAL_RC_FAST_D256, DEFAULT_32K_CLK_CAL_CYCLES));
        }
        return s_calibrated_freq.rc_fast_d256;
    case ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT:
        s_calibrated_freq.rc_fast_d256 = rtc_clk_freq_cal(rtc_clk_cal(CLK_CAL_RC_FAST_D256, DEFAULT_32K_CLK_CAL_CYCLES));
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
            s_calibrated_freq.xtal32k = rtc_clk_freq_cal(rtc_clk_cal(CLK_CAL_32K_XTAL, DEFAULT_32K_CLK_CAL_CYCLES));
        }
        return s_calibrated_freq.xtal32k;
    case ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT:
        s_calibrated_freq.xtal32k = rtc_clk_freq_cal(rtc_clk_cal(CLK_CAL_32K_XTAL, DEFAULT_32K_CLK_CAL_CYCLES));
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
            s_calibrated_freq.osc_slow = rtc_clk_freq_cal(rtc_clk_cal(CLK_CAL_32K_OSC_SLOW, DEFAULT_32K_CLK_CAL_CYCLES));
        }
        return s_calibrated_freq.osc_slow;
    case ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT:
        s_calibrated_freq.osc_slow = rtc_clk_freq_cal(rtc_clk_cal(CLK_CAL_32K_OSC_SLOW, DEFAULT_32K_CLK_CAL_CYCLES));
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
        s_calibrated_freq.rc_fast = rtc_clk_freq_cal(rtc_clk_cal(CLK_CAL_RC_FAST, DEFAULT_RC_FAST_CAL_CYCLES));
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
    case SOC_RTC_FAST_CLK_SRC_RC_FAST:
        return esp_clk_tree_rc_fast_get_freq_hz(precision) / clk_ll_rc_fast_get_divider();
#if SOC_CLK_LP_FAST_SUPPORT_LP_PLL
    case SOC_RTC_FAST_CLK_SRC_LP_PLL:
        return clk_ll_lp_pll_get_freq_mhz() * MHZ;
#endif
#if SOC_CLK_LP_FAST_SUPPORT_XTAL
    case SOC_RTC_FAST_CLK_SRC_XTAL:
        return clk_hal_xtal_get_freq_mhz() * MHZ;
#endif
#if SOC_CLK_LP_FAST_SUPPORT_XTAL_D2
    case SOC_RTC_FAST_CLK_SRC_XTAL_D2:
        return clk_hal_xtal_get_freq_mhz() * MHZ >> 1;
#endif
#if SOC_CLK_LP_FAST_SUPPORT_XTAL_D4
    case SOC_RTC_FAST_CLK_SRC_XTAL_D4:
        return clk_hal_xtal_get_freq_mhz() * MHZ >> 2;
#endif
    default:
        // Invalid clock source
        assert(false);
        return 0;
    }
}

#if SOC_CLK_APLL_SUPPORTED
// Current APLL frequency, in HZ. Zero if APLL is not enabled.
static portMUX_TYPE __attribute__((unused)) s_periph_apll_spinlock = portMUX_INITIALIZER_UNLOCKED;
static uint32_t s_cur_apll_freq_hz = 0;
static int s_apll_ref_cnt = 0;

// Pending APLL coefficients: calibration requires APLL to be powered on,
// so if freq is set before acquire, we defer rtc_clk_apll_coeff_set until enable.
static bool s_apll_coeff_pending = false;
static uint32_t s_apll_pending_o_div = 0;
static uint32_t s_apll_pending_sdm0 = 0;
static uint32_t s_apll_pending_sdm1 = 0;
static uint32_t s_apll_pending_sdm2 = 0;

void esp_clk_tree_apll_acquire(void)
{
    bool apply_pending = false;
    uint32_t o_div, sdm0, sdm1, sdm2;

    esp_os_enter_critical(&s_periph_apll_spinlock);
    s_apll_ref_cnt++;
    if (s_apll_ref_cnt == 1) {
        rtc_clk_apll_enable(true);
        if (s_apll_coeff_pending) {
            apply_pending = true;
            o_div = s_apll_pending_o_div;
            sdm0 = s_apll_pending_sdm0;
            sdm1 = s_apll_pending_sdm1;
            sdm2 = s_apll_pending_sdm2;
            s_apll_coeff_pending = false;
        }
    }
    esp_os_exit_critical(&s_periph_apll_spinlock);

    if (apply_pending) {
        rtc_clk_apll_coeff_set(o_div, sdm0, sdm1, sdm2);
    }
}

void esp_clk_tree_apll_release(void)
{
    esp_os_enter_critical(&s_periph_apll_spinlock);
    assert(s_apll_ref_cnt > 0);
    s_apll_ref_cnt--;
    if (s_apll_ref_cnt == 0) {
        s_cur_apll_freq_hz = 0;
        s_apll_coeff_pending = false;
        rtc_clk_apll_enable(false);
    }
    esp_os_exit_critical(&s_periph_apll_spinlock);
}

esp_err_t esp_clk_tree_apll_freq_set(uint32_t expt_freq_hz, uint32_t *real_freq_hz)
{
    uint32_t o_div = 0;
    uint32_t sdm0 = 0;
    uint32_t sdm1 = 0;
    uint32_t sdm2 = 0;

    uint32_t apll_freq = rtc_clk_apll_coeff_calc(expt_freq_hz, &o_div, &sdm0, &sdm1, &sdm2);
    ESP_RETURN_ON_FALSE(apll_freq, ESP_ERR_INVALID_ARG, TAG, "APLL coefficients calculate failed");
    bool need_config = true;
    bool apll_enabled = false;
    esp_os_enter_critical(&s_periph_apll_spinlock);
    /* If APLL is not in use or only one peripheral in use, its frequency can be changed as will
     * But when more than one peripheral refers APLL, its frequency is not allowed to change once it is set */
    if (s_cur_apll_freq_hz == 0 || s_apll_ref_cnt < 2) {
        s_cur_apll_freq_hz = apll_freq;
        apll_enabled = (s_apll_ref_cnt > 0);
        if (!apll_enabled) {
            // APLL not yet powered on, defer coeff_set (which includes calibration) until acquire
            s_apll_coeff_pending = true;
            s_apll_pending_o_div = o_div;
            s_apll_pending_sdm0 = sdm0;
            s_apll_pending_sdm1 = sdm1;
            s_apll_pending_sdm2 = sdm2;
        }
    } else {
        apll_freq = s_cur_apll_freq_hz;
        need_config = false;
    }
    esp_os_exit_critical(&s_periph_apll_spinlock);
    if (real_freq_hz != NULL) {
        *real_freq_hz = apll_freq;
    }
    if (need_config) {
        ESP_LOGD(TAG, "APLL works at %"PRIu32" Hz with coefficients [sdm0] %"PRIu32" [sdm1] %"PRIu32" [sdm2] %"PRIu32" [o_div] %"PRIu32"",
                 apll_freq, sdm0, sdm1, sdm2, o_div);
        if (apll_enabled) {
            rtc_clk_apll_coeff_set(o_div, sdm0, sdm1, sdm2);
        }
    } else {
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}
#endif /* SOC_CLK_APLL_SUPPORTED */

#if SOC_CLK_MPLL_SUPPORTED
static portMUX_TYPE __attribute__((unused)) s_periph_mpll_spinlock = portMUX_INITIALIZER_UNLOCKED;
static uint32_t s_cur_mpll_freq_hz = 0;
static int s_mpll_ref_cnt = 0;
#if CONFIG_ESP_LDO_RESERVE_PSRAM
static esp_ldo_channel_handle_t s_ldo_chan = NULL;
#endif

esp_err_t IRAM_ATTR esp_clk_tree_mpll_acquire(void)
{
    // power up LDO for the MPLL
#if CONFIG_ESP_LDO_RESERVE_PSRAM
    if (s_ldo_chan == NULL) {
        esp_ldo_channel_config_t ldo_mpll_config = {
            .chan_id = CONFIG_ESP_LDO_CHAN_PSRAM_DOMAIN,
            .voltage_mv = CONFIG_ESP_LDO_VOLTAGE_PSRAM_DOMAIN,
        };
        ESP_RETURN_ON_ERROR(esp_ldo_acquire_channel(&ldo_mpll_config, &s_ldo_chan), TAG, "acquire internal LDO for MPLL failed");
    }
#endif

    esp_os_enter_critical(&s_periph_mpll_spinlock);
    s_mpll_ref_cnt++;
    if (s_mpll_ref_cnt == 1) {
        // For the first time enable MPLL, need to set power up
        rtc_clk_mpll_enable();
    }
    esp_os_exit_critical(&s_periph_mpll_spinlock);
    return ESP_OK;
}

void esp_clk_tree_mpll_release(void)
{
    bool __attribute__((unused)) release_ldo = false;
    esp_os_enter_critical(&s_periph_mpll_spinlock);
    assert(s_mpll_ref_cnt > 0);
    s_mpll_ref_cnt--;
    if (s_mpll_ref_cnt == 0) {
        s_cur_mpll_freq_hz = 0;
        rtc_clk_mpll_disable();
        release_ldo = true;
    }
    esp_os_exit_critical(&s_periph_mpll_spinlock);

#if defined(CONFIG_ESP_LDO_CHAN_PSRAM_DOMAIN) && CONFIG_ESP_LDO_CHAN_PSRAM_DOMAIN != -1
    if (release_ldo && s_ldo_chan) {
        esp_ldo_release_channel(s_ldo_chan);
        s_ldo_chan = NULL;
    }
#endif
}

esp_err_t IRAM_ATTR esp_clk_tree_mpll_freq_set(uint32_t expt_freq_hz, uint32_t *real_freq_hz)
{
    esp_err_t ret = ESP_OK;

    // Guarantee 'esp_clk_tree_mpll_acquire' has been called before set mpll freq
    assert(s_mpll_ref_cnt > 0);

    esp_os_enter_critical(&s_periph_mpll_spinlock);
    if (s_cur_mpll_freq_hz == expt_freq_hz) {
        goto end;
    }
    /* If MPLL is not in use or only one peripheral in use, its frequency can be changed as will
     * But when more than one peripheral refers MPLL, its frequency is not allowed to change once it is set */
    if (s_cur_mpll_freq_hz == 0 || s_mpll_ref_cnt < 2) {
        uint32_t xtal_freq_mhz = clk_hal_xtal_get_freq_mhz();
        rtc_clk_mpll_configure(xtal_freq_mhz, expt_freq_hz / MHZ, false);
        s_cur_mpll_freq_hz = clk_ll_mpll_get_freq_mhz(xtal_freq_mhz) * MHZ;
    } else {
        ret = ESP_ERR_INVALID_STATE;
    }
end:
    if (real_freq_hz != NULL) {
        *real_freq_hz = s_cur_mpll_freq_hz;
    }
    esp_os_exit_critical(&s_periph_mpll_spinlock);
    return ret;
}
#endif /* SOC_CLK_MPLL_SUPPORTED */

/* ---------------------------------------------------------------------------
 * Generic derived-PLL clock engine. Target-agnostic: per-clock descriptor and
 * state come from `esp_clk_tree_get_derived_clk_desc()` (overridden per target
 * in port/<target>/esp_clk_tree.c). The engine never references any specific
 * clock id or chip cap; adding F25M / F20M / F80M / ... is purely a target-side
 * change.
 *
 * Public API entry points:
 *   - `esp_clk_tree_enable_src(clk, true/false)`     -> acquire / release
 *   - `esp_clk_tree_src_select_upstream(clk, src)`   -> mux selection
 *   - `esp_clk_tree_src_set_freq_hz(clk, hz, &real)` -> divider selection
 * The engine functions below are the internal targets the per-target
 * dispatchers route into.
 *
 * Semantics:
 *   - acquire: ref_cnt++; on first acquire enable the gate.
 *   - release: ref_cnt--; on last release disable the gate.
 *   - select_upstream: program the mux to source from `upstream`.
 *   - freq_set: pick a divider for `state->cur_upstream` (if `select_upstream`
 *               was called) or auto-pick an upstream that divides cleanly
 *               (if no upstream was selected); program the divider (and the
 *               mux on first call) atomically.
 * Share-lock (applies to select_upstream and freq_set when ref_cnt > 1):
 *   request must match the currently applied (upstream / divider) value;
 *   otherwise return ESP_ERR_INVALID_STATE and report the active frequency.
 * ------------------------------------------------------------------------- */
static portMUX_TYPE __attribute__((unused)) s_derived_clk_spinlock = portMUX_INITIALIZER_UNLOCKED;

__attribute__((weak)) const esp_clk_tree_derived_clk_desc_t *
esp_clk_tree_get_derived_clk_desc(soc_module_clk_t clk_src)
{
    (void)clk_src;
    return NULL;
}

esp_err_t esp_clk_tree_derived_clk_acquire(soc_module_clk_t clk_src)
{
    const esp_clk_tree_derived_clk_desc_t *desc = esp_clk_tree_get_derived_clk_desc(clk_src);
    if (desc == NULL || desc->state == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    esp_clk_tree_derived_clk_state_t *state = desc->state;

    esp_os_enter_critical(&s_derived_clk_spinlock);
    state->ref_cnt++;
    if (state->ref_cnt == 1) {
        desc->set_gate(true);
    }
    esp_os_exit_critical(&s_derived_clk_spinlock);
    return ESP_OK;
}

esp_err_t esp_clk_tree_derived_clk_release(soc_module_clk_t clk_src)
{
    const esp_clk_tree_derived_clk_desc_t *desc = esp_clk_tree_get_derived_clk_desc(clk_src);
    if (desc == NULL || desc->state == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    esp_clk_tree_derived_clk_state_t *state = desc->state;

    bool released_too_many = false;
    esp_os_enter_critical(&s_derived_clk_spinlock);
    if (state->ref_cnt <= 0) {
        state->ref_cnt = 0;
        released_too_many = true;
    } else {
        state->ref_cnt--;
        if (state->ref_cnt == 0) {
            desc->set_gate(false);
            state->cur_upstream = SOC_MOD_CLK_INVALID;
            state->cur_divider = 0;
        }
    }
    esp_os_exit_critical(&s_derived_clk_spinlock);
    if (released_too_many) {
        ESP_HW_LOGW(TAG, "derived clk %d released without matching acquire", (int)clk_src);
    }
    return ESP_OK;
}

/**
 * Iterate the descriptor's upstream candidates and pick the first one
 * (in preference order) whose currently programmed frequency is an integer
 * multiple of `expt_freq_hz`. When `preferred != SOC_MOD_CLK_INVALID`, only
 * that single candidate is considered.
 */
static esp_err_t s_derived_pick_config(const esp_clk_tree_derived_clk_desc_t *desc,
                                       soc_module_clk_t preferred,
                                       uint32_t expt_freq_hz,
                                       soc_module_clk_t *out_upstream,
                                       uint32_t *out_divider,
                                       uint8_t *out_mux_sel)
{
    bool any_match = false;
    for (size_t i = 0; i < desc->upstream_count; i++) {
        const esp_clk_tree_derived_upstream_t *cand = &desc->upstreams[i];
        if (preferred != SOC_MOD_CLK_INVALID && cand->upstream != preferred) {
            continue;
        }
        any_match = true;
        uint32_t up_hz = 0;
        if (esp_clk_tree_src_get_freq_hz(cand->upstream,
                                         ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX,
                                         &up_hz) != ESP_OK || up_hz == 0) {
            continue;
        }
        if (up_hz % expt_freq_hz == 0) {
            *out_upstream = cand->upstream;
            *out_divider  = up_hz / expt_freq_hz;
            *out_mux_sel  = cand->mux_sel;
            return ESP_OK;
        }
    }
    // Distinguish "not in allowed list" from "in list but doesn't divide cleanly"
    // so callers get a more useful error.
    return (preferred != SOC_MOD_CLK_INVALID && !any_match) ? ESP_ERR_INVALID_ARG
                                                            : ESP_ERR_NOT_FOUND;
}

esp_err_t esp_clk_tree_derived_clk_freq_set(soc_module_clk_t clk_src,
                                            uint32_t expt_freq_hz,
                                            uint32_t *real_freq_hz)
{
    const esp_clk_tree_derived_clk_desc_t *desc = esp_clk_tree_get_derived_clk_desc(clk_src);
    if (desc == NULL || desc->state == NULL || desc->upstreams == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    esp_clk_tree_derived_clk_state_t *state = desc->state;
    ESP_RETURN_ON_FALSE(expt_freq_hz > 0, ESP_ERR_INVALID_ARG, TAG, "freq must be > 0");

    // If `esp_clk_tree_src_select_upstream()` was called, honor that choice;
    // otherwise auto-pick the first upstream in the descriptor that divides
    // cleanly to the requested frequency.
    soc_module_clk_t preferred = state->cur_upstream;
    soc_module_clk_t upstream = SOC_MOD_CLK_INVALID;
    uint32_t divider = 0;
    uint8_t mux_sel = 0;
    ESP_RETURN_ON_ERROR(s_derived_pick_config(desc, preferred, expt_freq_hz,
                                              &upstream, &divider, &mux_sel),
                        TAG,
                        "no upstream divides cleanly to %" PRIu32 " Hz for derived clk %d",
                        expt_freq_hz, (int)clk_src);

    esp_err_t ret = ESP_OK;
    soc_module_clk_t reported_upstream = SOC_MOD_CLK_INVALID;
    uint32_t reported_divider = 0;

    esp_os_enter_critical(&s_derived_clk_spinlock);
    // Caller must have acquired the clock first.
    assert(state->ref_cnt > 0);

    bool same_config = (state->cur_upstream == upstream) &&
                        (state->cur_divider == divider);
    // Allow the first peer to commit a divider even when ref_cnt > 1
    // (e.g. another peer ran enable_src already but hasn't called freq_set
    // yet). Mirrors MPLL's `cur == 0 || ref_cnt < 2` check.
    bool first_commit = (state->cur_divider == 0);
    if (state->ref_cnt < 2 || same_config || first_commit) {
        // First-time configuration: also program the mux. When the caller
        // already invoked `select_upstream`, `state->cur_upstream` already
        // matches `upstream` so the mux is left untouched.
        if (state->cur_upstream != upstream && desc->set_src != NULL) {
            desc->set_src(mux_sel);
        }
        desc->set_divider(divider);
        state->cur_upstream = upstream;
        state->cur_divider = divider;
    } else {
        ret = ESP_ERR_INVALID_STATE;
    }
    esp_os_exit_critical(&s_derived_clk_spinlock);
    reported_upstream = state->cur_upstream;
    reported_divider = state->cur_divider;

    if (real_freq_hz != NULL) {
        uint32_t up_hz = 0;
        if (reported_upstream != SOC_MOD_CLK_INVALID && reported_divider != 0 &&
            esp_clk_tree_src_get_freq_hz(reported_upstream,
                                         ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX,
                                         &up_hz) == ESP_OK) {
            *real_freq_hz = up_hz / reported_divider;
        } else {
            *real_freq_hz = 0;
        }
    }
    return ret;
}

esp_err_t esp_clk_tree_src_select_upstream(soc_module_clk_t clk_src,
                                           soc_module_clk_t upstream)
{
    const esp_clk_tree_derived_clk_desc_t *desc = esp_clk_tree_get_derived_clk_desc(clk_src);
    if (desc == NULL || desc->state == NULL || desc->upstreams == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    ESP_RETURN_ON_FALSE(upstream > 0 && upstream < SOC_MOD_CLK_INVALID,
                        ESP_ERR_INVALID_ARG, TAG, "invalid upstream clk %d", (int)upstream);

    // Validate `upstream` is in the descriptor's allowed list and recover its
    // mux selector value.
    uint8_t mux_sel = 0;
    bool found = false;
    for (size_t i = 0; i < desc->upstream_count; i++) {
        if (desc->upstreams[i].upstream == upstream) {
            mux_sel = desc->upstreams[i].mux_sel;
            found = true;
            break;
        }
    }
    ESP_RETURN_ON_FALSE(found, ESP_ERR_INVALID_ARG, TAG,
                        "upstream %d not allowed for derived clk %d",
                        (int)upstream, (int)clk_src);

    esp_clk_tree_derived_clk_state_t *state = desc->state;
    esp_err_t ret = ESP_OK;
    esp_os_enter_critical(&s_derived_clk_spinlock);
    // Caller must have acquired the clock first.
    assert(state->ref_cnt > 0);

    // Allow the first peer to commit an upstream even when ref_cnt > 1.
    bool first_commit = (state->cur_upstream == SOC_MOD_CLK_INVALID);
    if (state->ref_cnt > 1 && !first_commit && state->cur_upstream != upstream) {
        // Another peer is already using this clock with a different upstream.
        ret = ESP_ERR_INVALID_STATE;
    } else if (state->cur_upstream != upstream) {
        if (desc->set_src != NULL) {
            desc->set_src(mux_sel);
        }
        state->cur_upstream = upstream;
        // Divider for the previous upstream is no longer meaningful; the next
        // `set_freq_hz` call will program a fresh divider for `upstream`.
        state->cur_divider = 0;
    }
    esp_os_exit_critical(&s_derived_clk_spinlock);
    return ret;
}
