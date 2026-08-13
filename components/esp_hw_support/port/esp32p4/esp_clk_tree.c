/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "esp_clk_tree.h"
#include "esp_attr.h"
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
#include "esp_private/critical_section.h"

ESP_LOG_ATTR_TAG(TAG, "esp_clk_tree");

// ESP32-P4 PLL_F50M is fixed-source (MPLL only, no upstream mux); set_src is
// NULL and the mux_sel value is ignored by the engine.
static const esp_clk_tree_derived_upstream_t s_pll_f50m_upstreams[] = {
    { SOC_MOD_CLK_MPLL, 0 },
};

static esp_clk_tree_derived_clk_state_t s_pll_f50m_state = {
    .ref_cnt      = 0,
    .cur_upstream = SOC_MOD_CLK_INVALID,
    .cur_divider  = 0,
};

static const esp_clk_tree_derived_clk_desc_t s_pll_f50m_desc = {
    .clk_id         = SOC_MOD_CLK_PLL_F50M,
    .set_src        = NULL,
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
        clk_src_freq = clk_hal_xtal_get_freq_mhz() * MHZ;
        break;
    case SOC_MOD_CLK_SYS:
        clk_src_freq = clk_hal_sys_get_freq_hz();
        break;
    case SOC_MOD_CLK_APB:
        clk_src_freq = clk_hal_apb_get_freq_hz();
        break;
    case SOC_MOD_CLK_PLL_F20M:
        clk_src_freq = CLK_LL_PLL_480M_FREQ_MHZ / clk_ll_pll_f20m_get_divider() * MHZ;
        break;
    case SOC_MOD_CLK_PLL_F80M:
        clk_src_freq = CLK_LL_PLL_80M_FREQ_MHZ * MHZ;
        break;
    case SOC_MOD_CLK_PLL_F120M:
        clk_src_freq = CLK_LL_PLL_120M_FREQ_MHZ * MHZ;
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
    case SOC_MOD_CLK_SPLL:
        clk_src_freq = CLK_LL_PLL_480M_FREQ_MHZ * MHZ;
        break;
    case SOC_MOD_CLK_MPLL:
        clk_src_freq = clk_ll_mpll_get_freq_mhz(clk_hal_xtal_get_freq_mhz()) * MHZ;
        break;
    case SOC_MOD_CLK_APLL:
        clk_src_freq = clk_hal_apll_get_freq_hz();
        break;
    case SOC_MOD_CLK_SDIO_PLL:
        clk_src_freq = CLK_LL_PLL_SDIO_FREQ_MHZ * MHZ;
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
    case SOC_MOD_CLK_LP_PLL:
        clk_src_freq = clk_ll_lp_pll_get_freq_mhz() * MHZ;
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

DEFINE_CRIT_SECTION_LOCK_STATIC(s_clk_tree_spinlock);

/** Per soc_module_clk_t: record clock gate consumers */
static int16_t s_mod_clk_gate_ref_cnt[SOC_MOD_CLK_INVALID] = { 0 };

static bool s_clk_tree_initialized = false;

void esp_clk_tree_initialize(void)
{
    soc_reset_reason_t rst_reason = esp_rom_get_reset_reason(0);
    if ((rst_reason == RESET_REASON_CPU_SW) || (rst_reason == RESET_REASON_CPU_MWDT)          \
            || (rst_reason == RESET_REASON_CPU_RWDT) || (rst_reason == RESET_REASON_CPU_JTAG) \
            || (rst_reason == RESET_REASON_CPU_LOCKUP)) {
        s_clk_tree_initialized = true;
        return;
    }

    _clk_gate_ll_ref_20m_clk_en(false);
    _clk_gate_ll_ref_25m_clk_en(false);
    _clk_gate_ll_ref_50m_clk_en(false);
    _clk_gate_ll_ref_80m_clk_en(false);
    _clk_gate_ll_ref_120m_clk_en(false);
#if !CONFIG_ESP_ENABLE_PVT
    // PLL_F160M must always on if PVT is enabled.
    _clk_gate_ll_ref_160m_clk_en(false);
#endif
    _clk_gate_ll_ref_240m_clk_en(false);
    s_clk_tree_initialized = true;
}

bool esp_clk_tree_port_is_power_on(soc_root_clk_circuit_t clk_circuit)
{
    (void)clk_circuit;
    return false;
}

bool esp_clk_tree_enable_power(soc_root_clk_circuit_t clk_circuit, bool enable)
{
    (void)clk_circuit; (void)enable;
    return false; // TODO: PM-653
}

/* -------------------------------------------------------------------------- */
/* Fixed ref clocks: gate only (no parent power management on ESP32-P4)       */
/* -------------------------------------------------------------------------- */

typedef void (*esp_clk_tree_gate_fn_t)(bool enable);

typedef struct {
    soc_module_clk_t       clk_id;
    esp_clk_tree_gate_fn_t set_gate;
} esp_clk_tree_gated_clk_t;

static void esp_clk_tree_gate_rc_fast(bool enable)
{
    if (enable) {
        rtc_dig_clk8m_enable();
    } else {
        rtc_dig_clk8m_disable();
    }
}

typedef enum {
    ESP_CLK_TREE_GATED_CLK_RC_FAST,
    ESP_CLK_TREE_GATED_CLK_PLL_F20M,
    ESP_CLK_TREE_GATED_CLK_PLL_F25M,
    ESP_CLK_TREE_GATED_CLK_PLL_F80M,
    ESP_CLK_TREE_GATED_CLK_PLL_F120M,
    ESP_CLK_TREE_GATED_CLK_PLL_F160M,
    ESP_CLK_TREE_GATED_CLK_PLL_F240M,
    ESP_CLK_TREE_GATED_CLK_NUM,
} esp_clk_tree_gated_clk_id_t;

static const esp_clk_tree_gated_clk_t s_gated_ref_clks[] = {
    [ESP_CLK_TREE_GATED_CLK_RC_FAST]    = { SOC_MOD_CLK_RC_FAST,    esp_clk_tree_gate_rc_fast },
    [ESP_CLK_TREE_GATED_CLK_PLL_F20M]   = { SOC_MOD_CLK_PLL_F20M,   _clk_gate_ll_ref_20m_clk_en },
    [ESP_CLK_TREE_GATED_CLK_PLL_F25M]   = { SOC_MOD_CLK_PLL_F25M,   _clk_gate_ll_ref_25m_clk_en },
    [ESP_CLK_TREE_GATED_CLK_PLL_F80M]   = { SOC_MOD_CLK_PLL_F80M,   _clk_gate_ll_ref_80m_clk_en },
    [ESP_CLK_TREE_GATED_CLK_PLL_F120M]  = { SOC_MOD_CLK_PLL_F120M,  _clk_gate_ll_ref_120m_clk_en },
    [ESP_CLK_TREE_GATED_CLK_PLL_F160M]  = { SOC_MOD_CLK_PLL_F160M,  _clk_gate_ll_ref_160m_clk_en },
    [ESP_CLK_TREE_GATED_CLK_PLL_F240M]  = { SOC_MOD_CLK_PLL_F240M,  _clk_gate_ll_ref_240m_clk_en },
};

#define ENABLE_CLK_GATE(clk_src_en_func, enable) \
    do { \
        if ((clk_src_en_func) != NULL) { \
            PERIPH_RCC_ATOMIC() { \
                (clk_src_en_func)(enable); \
            }; \
        } \
    } while (0)

FORCE_INLINE_ATTR esp_err_t esp_clk_tree_enable_gated_clk(const esp_clk_tree_gated_clk_t *entry, bool enable)
{
    int16_t prev_ref_cnt;

    esp_os_enter_critical(&s_clk_tree_spinlock);
    if (enable) {
        prev_ref_cnt = s_mod_clk_gate_ref_cnt[entry->clk_id]++;
    } else {
        prev_ref_cnt = s_mod_clk_gate_ref_cnt[entry->clk_id]--;
        if (prev_ref_cnt <= 0) {
            s_mod_clk_gate_ref_cnt[entry->clk_id] = 0;
            esp_os_exit_critical(&s_clk_tree_spinlock);
            ESP_EARLY_LOGW(TAG, "soc_module_clk_t %d disabled multiple times!!", entry->clk_id);
            return ESP_OK;
        }
    }
    esp_os_exit_critical(&s_clk_tree_spinlock);

    if (prev_ref_cnt == 0 && enable) {
        ENABLE_CLK_GATE(entry->set_gate, true);
    } else if (prev_ref_cnt == 1 && !enable) {
        ENABLE_CLK_GATE(entry->set_gate, false);
    }
    return ESP_OK;
}

esp_err_t esp_clk_tree_enable_src(soc_module_clk_t clk_src, bool enable)
{
    if (clk_src < 1 || clk_src >= SOC_MOD_CLK_INVALID || clk_src == SOC_MOD_CLK_XTAL) {
        /* Not managed by esp_clk_tree */
        return ESP_OK;
    }

    if (!s_clk_tree_initialized) {
        return ESP_OK;
    }

    esp_clk_tree_gated_clk_id_t gated_clk_id;
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
            return ESP_OK;
        }
    case SOC_MOD_CLK_RC_FAST:   gated_clk_id = ESP_CLK_TREE_GATED_CLK_RC_FAST;   break;
    case SOC_MOD_CLK_PLL_F20M:  gated_clk_id = ESP_CLK_TREE_GATED_CLK_PLL_F20M;  break;
    case SOC_MOD_CLK_PLL_F25M:  gated_clk_id = ESP_CLK_TREE_GATED_CLK_PLL_F25M;  break;
    case SOC_MOD_CLK_PLL_F80M:  gated_clk_id = ESP_CLK_TREE_GATED_CLK_PLL_F80M;  break;
    case SOC_MOD_CLK_PLL_F120M: gated_clk_id = ESP_CLK_TREE_GATED_CLK_PLL_F120M; break;
#if !CONFIG_ESP_ENABLE_PVT
    // PLL_F160M must always on if PVT is enabled.
    case SOC_MOD_CLK_PLL_F160M: gated_clk_id = ESP_CLK_TREE_GATED_CLK_PLL_F160M; break;
#endif
    case SOC_MOD_CLK_PLL_F240M: gated_clk_id = ESP_CLK_TREE_GATED_CLK_PLL_F240M; break;
    default:
        // Derived PLL clocks (PLL_F50M, ...) that participate in the shared
        // refcount/lock engine route through that engine instead of the
        // global s_pll_src_cg_ref_cnt array below.
        if (esp_clk_tree_get_derived_clk_desc(clk_src) != NULL) {
            return enable ? esp_clk_tree_derived_clk_acquire(clk_src)
                          : esp_clk_tree_derived_clk_release(clk_src);
        }
        return ESP_OK;
    }
    return esp_clk_tree_enable_gated_clk(&s_gated_ref_clks[gated_clk_id], enable);
}
