/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdatomic.h>
#include "sdkconfig.h"
#include "esp_clk_tree.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_log.h"
#include "soc/clk_tree_defs.h"
#include "soc/rtc.h"
#include "soc/reset_reasons.h"
#include "soc/soc_caps.h"
#include "hal/clk_gate_ll.h"
#include "hal/clk_tree_hal.h"
#include "hal/clk_tree_ll.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/esp_clk_tree_derived.h"
#include "esp_private/periph_ctrl.h"

ESP_LOG_ATTR_TAG(TAG, "esp_clk_tree");

// Allowed upstreams for PLL_F50M, in preference order (used by both auto-pick
// and explicit-upstream paths). Mux selectors match the HP_SYS_CLKRST
// `reg_ref_500m_sel` field: 0 = CPLL, 1 = MPLL.
static const esp_clk_tree_derived_upstream_t s_pll_f50m_upstreams[] = {
    { SOC_MOD_CLK_CPLL, 0 },
    { SOC_MOD_CLK_MPLL, 1 },
};

static esp_clk_tree_derived_clk_state_t s_pll_f50m_state = {
    .ref_cnt      = 0,
    .cur_upstream = SOC_MOD_CLK_INVALID,
    .cur_divider  = 0,
};

static const esp_clk_tree_derived_clk_desc_t s_pll_f50m_desc = {
    .clk_id         = SOC_MOD_CLK_PLL_F50M,
    .set_src        = clk_ll_ref_500m_set_src,
    .set_divider    = clk_ll_pll_f50m_set_divider,
    .set_gate       = _clk_gate_ll_ref_50m_clk_en, // using RCC_ATOMIC lock free function version to avoid nesting critical sections
    .upstreams      = s_pll_f50m_upstreams,
    .upstream_count = sizeof(s_pll_f50m_upstreams) / sizeof(s_pll_f50m_upstreams[0]),
    .state          = &s_pll_f50m_state,
};

const esp_clk_tree_derived_clk_desc_t *esp_clk_tree_get_derived_clk_desc(soc_module_clk_t clk_src)
{
    switch (clk_src) {
    case SOC_MOD_CLK_PLL_F50M:
        return &s_pll_f50m_desc;
    default:
        return NULL;
    }
}

esp_err_t esp_clk_tree_src_get_freq_hz(soc_module_clk_t clk_src, esp_clk_tree_src_freq_precision_t precision,
                                       uint32_t *freq_value)
{
    ESP_RETURN_ON_FALSE(clk_src > 0 && clk_src < SOC_MOD_CLK_INVALID, ESP_ERR_INVALID_ARG, TAG, "unknown clk src");
    ESP_RETURN_ON_FALSE(precision < ESP_CLK_TREE_SRC_FREQ_PRECISION_INVALID, ESP_ERR_INVALID_ARG, TAG, "unknown precision");
    ESP_RETURN_ON_FALSE(freq_value, ESP_ERR_INVALID_ARG, TAG, "null pointer");

    uint32_t clk_src_freq = 0;
    switch (clk_src) {
    case SOC_MOD_CLK_CPU:
        clk_src_freq = clk_hal_cpu_get_freq_hz();
        break;
    case SOC_MOD_CLK_XTAL:
        clk_src_freq = SOC_XTAL_FREQ_40M * MHZ;
        break;
    case SOC_MOD_CLK_SYS:
        clk_src_freq = clk_hal_sys_get_freq_hz();
        break;
    case SOC_MOD_CLK_PLL_F20M:
        clk_src_freq = CLK_LL_PLL_480M_FREQ_MHZ / clk_ll_pll_f20m_get_divider() * MHZ;
        break;
    case SOC_MOD_CLK_PLL_F80M:
        clk_src_freq = CLK_LL_PLL_80M_FREQ_MHZ * MHZ;
        break;
    case SOC_MOD_CLK_PLL_F160M:
        clk_src_freq = CLK_LL_PLL_160M_FREQ_MHZ * MHZ;
        break;
    case SOC_MOD_CLK_PLL_F240M:
        clk_src_freq = CLK_LL_PLL_240M_FREQ_MHZ * MHZ;
        break;
    case SOC_MOD_CLK_CPLL:
        clk_src_freq = clk_ll_cpll_get_freq_mhz(clk_hal_xtal_get_freq_mhz()) * MHZ;
        break;
    case SOC_MOD_CLK_BBPLL:
        clk_src_freq = CLK_LL_PLL_480M_FREQ_MHZ * MHZ;
        break;
    case SOC_MOD_CLK_MPLL:
        clk_src_freq = clk_ll_mpll_get_freq_mhz(clk_hal_xtal_get_freq_mhz()) * MHZ;
        break;
    case SOC_MOD_CLK_APLL:
        clk_src_freq = clk_hal_apll_get_freq_hz();
        break;
    case SOC_MOD_CLK_RTC_SLOW:
        clk_src_freq = esp_clk_tree_lp_slow_get_freq_hz(precision);
        break;
    case SOC_MOD_CLK_RTC_FAST:
    case SOC_MOD_CLK_LP_DYN_FAST: // This clock can be derived from RTC_SLOW_CLK or RTC_FAST_CLK depending on the chip’s power mode.
                                  // However, this function is only supposed to run under active mode, so its frequency is the same as RTC_FAST_CLK.
        clk_src_freq = esp_clk_tree_lp_fast_get_freq_hz(precision);
        break;
    case SOC_MOD_CLK_RC_FAST:
        clk_src_freq = esp_clk_tree_rc_fast_get_freq_hz(precision);
        break;
    case SOC_MOD_CLK_XTAL32K:
        clk_src_freq = esp_clk_tree_xtal32k_get_freq_hz(precision);
        break;
    case SOC_MOD_CLK_XTAL_D2:
        clk_src_freq = (clk_hal_xtal_get_freq_mhz() * MHZ) >> 1;
        break;
    case SOC_MOD_CLK_APB:
        clk_src_freq = clk_hal_apb_get_freq_hz();
        break;
    default:
        break;
    }

    ESP_RETURN_ON_FALSE(clk_src_freq, ESP_FAIL, TAG,
                        "freq shouldn't be 0, calibration failed");
    *freq_value = clk_src_freq;
    return ESP_OK;
}

esp_err_t esp_clk_tree_src_set_freq_hz(soc_module_clk_t clk_src, uint32_t expt_freq_value, uint32_t *ret_freq_value)
{
    ESP_RETURN_ON_FALSE(clk_src > 0 && clk_src < SOC_MOD_CLK_INVALID, ESP_ERR_INVALID_ARG, TAG, "unknown clk src");
    ESP_RETURN_ON_FALSE(expt_freq_value > 0, ESP_ERR_INVALID_ARG, TAG, "invalid frequency");

    // Derived PLL clocks (PLL_F50M, ...) are configured by the shared engine,
    // which picks an upstream + divider yielding `expt_freq_value`.
    if (esp_clk_tree_get_derived_clk_desc(clk_src) != NULL) {
        return esp_clk_tree_derived_clk_freq_set(clk_src, expt_freq_value, ret_freq_value);
    }

    uint32_t real_freq_value = 0;
    esp_err_t ret = ESP_OK;
    switch (clk_src) {
    case SOC_MOD_CLK_APLL:
        ret = esp_clk_tree_apll_freq_set(expt_freq_value, &real_freq_value);
        break;
    case SOC_MOD_CLK_MPLL:
        ret = esp_clk_tree_mpll_freq_set(expt_freq_value, &real_freq_value);
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (ret_freq_value) {
        *ret_freq_value = real_freq_value;
    }
    return ret;
}

static _Atomic int16_t s_pll_src_cg_ref_cnt[SOC_MOD_CLK_INVALID] = { 0 };
static bool s_clk_tree_initialized = false;
static int16_t s_cpll_ref_cnt = 0;

void esp_clk_tree_initialize(void)
{
    // TODO: IDF-15502
    /*soc_reset_reason_t rst_reason = esp_rom_get_reset_reason(0);
    if ((rst_reason == RESET_REASON_CPU_SW) || (rst_reason == RESET_REASON_CPU_MWDT)          \
            || (rst_reason == RESET_REASON_CPU_RWDT) || (rst_reason == RESET_REASON_CPU_JTAG) \
            || (rst_reason == RESET_REASON_CPU_LOCKUP)) {
        s_clk_tree_initialized = true;
        return;
    }*/

    // Power
    soc_cpu_clk_src_t cpu_clk_src_btld = clk_ll_cpu_get_src();
    if (cpu_clk_src_btld == SOC_CPU_CLK_SRC_CPLL) {
        s_cpll_ref_cnt++;
    } else if (cpu_clk_src_btld == SOC_CPU_CLK_SRC_PLL_F240M) {
        // TODO: IDF-15502
        s_pll_src_cg_ref_cnt[SOC_MOD_CLK_PLL_F240M] = 1;
    }

    // flash clock source is set to BBPLL in bootloader
    s_pll_src_cg_ref_cnt[SOC_MOD_CLK_BBPLL] = 1;

    // Gating: disable all PLL-derived reference clocks; they will be re-enabled on demand via esp_clk_tree_enable_src
    // TODO: IDF-15502
    //_clk_gate_ll_ref_20m_clk_en(false);
    //_clk_gate_ll_ref_25m_clk_en(false);
    //_clk_gate_ll_ref_50m_clk_en(false);
    //_clk_gate_ll_ref_80m_clk_en(false);
    //_clk_gate_ll_ref_160m_clk_en(false);
    //if (s_pll_src_cg_ref_cnt[SOC_MOD_CLK_PLL_F240M] == 0) _clk_gate_ll_ref_240m_clk_en(false);
    s_clk_tree_initialized = true;
}

bool esp_clk_tree_enable_power(soc_root_clk_circuit_t clk_circuit, bool enable)
{
    bool toggled = false;
    switch (clk_circuit) {
    case SOC_ROOT_CIRCUIT_CLK_CPLL:
        if (enable) {
            s_cpll_ref_cnt++;
        } else {
            s_cpll_ref_cnt--;
        }

        // Note that a calibration is usually needed after enabling CPLL
        if (s_cpll_ref_cnt == 1) {
            clk_ll_cpll_enable();
            toggled = true;
        } else if (s_cpll_ref_cnt == 0) {
            clk_ll_cpll_disable();
            toggled = true;
        }

        assert(s_cpll_ref_cnt >= 0);
        break;
    default:
        break;
    }
    return toggled;
}

#define ENABLE_CLK_GATE(clk_src_en_func, enable) \
    PERIPH_RCC_ATOMIC() { \
        clk_src_en_func(enable); \
    }

esp_err_t esp_clk_tree_enable_src(soc_module_clk_t clk_src, bool enable)
{
    if (clk_src < 1 || clk_src >= SOC_MOD_CLK_INVALID) {
        // some conditions is legal, e.g. -1 means external clock source
        return ESP_OK;
    }

    if (!s_clk_tree_initialized) {
        return ESP_OK;
    }

    int16_t prev_ref_cnt = 0;

    // Derived PLL clocks (PLL_F50M, ...) that participate in the shared
    // refcount/lock engine route through that engine instead of the
    // global s_pll_src_cg_ref_cnt array below.
    if (esp_clk_tree_get_derived_clk_desc(clk_src) != NULL) {
        return enable ? esp_clk_tree_derived_clk_acquire(clk_src)
                      : esp_clk_tree_derived_clk_release(clk_src);
    }

    // these clock sources have their own reference counting
    switch (clk_src) {
        case SOC_MOD_CLK_APLL:
            if (enable) {
                esp_clk_tree_apll_acquire();
            } else {
                esp_clk_tree_apll_release();
            }
            return ESP_OK;
        case SOC_MOD_CLK_MPLL:
            if (enable) {
                return esp_clk_tree_mpll_acquire();
            } else {
                esp_clk_tree_mpll_release();
            }
            return ESP_OK;
        default:
            break;
    }

    // other clock sources use the global reference counting
    if (enable) {
        prev_ref_cnt = atomic_fetch_add(&s_pll_src_cg_ref_cnt[clk_src], 1);
    } else {
        prev_ref_cnt = atomic_fetch_sub(&s_pll_src_cg_ref_cnt[clk_src], 1);
        if (prev_ref_cnt <= 0) {
            ESP_EARLY_LOGW(TAG, "soc_module_clk_t %d disabled multiple times!!", clk_src);
            atomic_store(&s_pll_src_cg_ref_cnt[clk_src], 0);
            return ESP_OK;
        }
    }
    // TODO: IDF-15502
    //if ((prev_ref_cnt == 0 && enable) || (prev_ref_cnt == 1 && !enable)) {
    //    switch (clk_src) {
    //        case SOC_MOD_CLK_RC_FAST:   enable ? rtc_dig_clk8m_enable() : rtc_dig_clk8m_disable(); break;
    //        case SOC_MOD_CLK_PLL_F20M:  ENABLE_CLK_GATE(clk_gate_ll_ref_20m_clk_en, enable);  break;
    //        case SOC_MOD_CLK_PLL_F25M:  ENABLE_CLK_GATE(clk_gate_ll_ref_25m_clk_en, enable);  break;
    //        case SOC_MOD_CLK_PLL_F80M:  ENABLE_CLK_GATE(clk_gate_ll_ref_80m_clk_en, enable);  break;
    //        case SOC_MOD_CLK_PLL_F160M: ENABLE_CLK_GATE(clk_gate_ll_ref_160m_clk_en, enable); break;
    //        case SOC_MOD_CLK_PLL_F240M: ENABLE_CLK_GATE(clk_gate_ll_ref_240m_clk_en, enable); break;
    //        default: break;
    //    }
    //}

    return ESP_OK;
}
