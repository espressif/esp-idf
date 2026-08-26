/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <assert.h>
#include "sdkconfig.h"
#include "esp_clk_tree.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "soc/rtc.h"
#include "soc/reset_reasons.h"
#include "hal/clk_gate_ll.h"
#include "hal/clk_tree_hal.h"
#include "hal/clk_tree_ll.h"
#include "hal/mspi_ll.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/esp_clk_tree_derived.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/critical_section.h"

ESP_LOG_ATTR_TAG(TAG, "esp_clk_tree");

/* -------------------------------------------------------------------------- */
/* Derived clocks: configurable divider and/or upstream mux + multi-user lock */
/* -------------------------------------------------------------------------- */

enum {
    ESP_CLK_TREE_DERIVED_PLL_F25M = 0,
    ESP_CLK_TREE_DERIVED_PLL_F50M,
    ESP_CLK_TREE_DERIVED_REF_F80M,
    ESP_CLK_TREE_DERIVED_PLL_NUM,
};

static esp_clk_tree_derived_clk_state_t s_derived_pll_states[ESP_CLK_TREE_DERIVED_PLL_NUM];

static int8_t s_ref_500m_committed_mux = -1;

/**
 * Power ref_25m/ref_50m shared upstream (ref_500m_sel: 0 = CPLL, 1 = MPLL).
 * @param mux_sel  0 or 1; values < 0 are ignored.
 */
static esp_err_t esp_clk_tree_ref_500m_parent_power(int8_t mux_sel, bool enable)
{
    if (mux_sel < 0) {
        return ESP_OK;
    }
    if (mux_sel == 0) {
        esp_clk_tree_enable_power(SOC_ROOT_CIRCUIT_CLK_CPLL, enable);
        return ESP_OK;
    }
    if (enable) {
        return esp_clk_tree_mpll_acquire();
    }
    esp_clk_tree_mpll_release();
    return ESP_OK;
}

/**
 * Shared ref_500m_sel: PLL_F25M and PLL_F50M must use the same upstream when both are on.
 */
static void esp_clk_tree_ref_500m_set_src(uint8_t mux_sel)
{
    if (s_derived_pll_states[ESP_CLK_TREE_DERIVED_PLL_F25M].ref_cnt > 0
            && s_derived_pll_states[ESP_CLK_TREE_DERIVED_PLL_F50M].ref_cnt > 0
            && s_ref_500m_committed_mux >= 0) {
        assert(mux_sel == (uint8_t)s_ref_500m_committed_mux);
    }
    clk_ll_ref_500m_set_src(mux_sel);
}

static esp_err_t esp_clk_tree_ref_500m_derived_acquire_parent(void)
{
    int8_t mux = (int8_t)clk_ll_ref_500m_get_src();
    if (mux == s_ref_500m_committed_mux) {
        return ESP_OK;
    }
    if (s_derived_pll_states[ESP_CLK_TREE_DERIVED_PLL_F25M].ref_cnt > 0
            && s_derived_pll_states[ESP_CLK_TREE_DERIVED_PLL_F50M].ref_cnt > 0
            && s_ref_500m_committed_mux >= 0) {
        assert(mux == s_ref_500m_committed_mux);
    }
    esp_clk_tree_ref_500m_parent_power(s_ref_500m_committed_mux, false);
    esp_err_t ret = esp_clk_tree_ref_500m_parent_power(mux, true);
    if (ret != ESP_OK) {
        esp_clk_tree_ref_500m_parent_power(s_ref_500m_committed_mux, true);
        return ret;
    }
    s_ref_500m_committed_mux = mux;
    return ESP_OK;
}

static esp_err_t esp_clk_tree_ref_500m_derived_release_parent(void)
{
    if (s_derived_pll_states[ESP_CLK_TREE_DERIVED_PLL_F25M].ref_cnt > 0
            || s_derived_pll_states[ESP_CLK_TREE_DERIVED_PLL_F50M].ref_cnt > 0) {
        return ESP_OK;
    }
    esp_clk_tree_ref_500m_parent_power(s_ref_500m_committed_mux, false);
    s_ref_500m_committed_mux = -1;
    return ESP_OK;
}

/** F80M parent path committed at first enable: 0 = BBPLL, 1 = XTALx2 (-1 = none). */
static int8_t s_ref_80m_committed_sel = -1;

/**
 * REF_F80M enable-time source policy:
 * 1) If BBPLL is already on → BBPLL/6 path + one more BBPLL power user.
 * 2) Else → power XTALx2 and select ref_80m via clk_ll_ref_80m_set_src(1).
 */
static esp_err_t esp_clk_tree_ref_80m_derived_acquire_parent(void)
{
    if (s_ref_80m_committed_sel >= 0) {
        return ESP_OK;
    }

    _clk_gate_ll_ref_80m_mux_clk_en(true);
    if (esp_clk_tree_port_is_power_on(SOC_ROOT_CIRCUIT_CLK_BBPLL)) {
        esp_clk_tree_enable_power(SOC_ROOT_CIRCUIT_CLK_BBPLL, true);
        clk_ll_ref_80m_set_src(0);
        s_ref_80m_committed_sel = 0;
    } else {
        esp_clk_tree_enable_power(SOC_ROOT_CIRCUIT_CLK_XTAL_X2, true);
        clk_ll_ref_80m_set_src(1);
        s_ref_80m_committed_sel = 1;
    }
    return ESP_OK;
}

static esp_err_t esp_clk_tree_ref_80m_derived_release_parent(void)
{
    if (s_derived_pll_states[ESP_CLK_TREE_DERIVED_REF_F80M].ref_cnt > 0) {
        return ESP_OK;
    }
    if (s_ref_80m_committed_sel == 0) {
        esp_clk_tree_enable_power(SOC_ROOT_CIRCUIT_CLK_BBPLL, false);
    } else if (s_ref_80m_committed_sel == 1) {
        esp_clk_tree_enable_power(SOC_ROOT_CIRCUIT_CLK_XTAL_X2, false);
    }
    _clk_gate_ll_ref_80m_mux_clk_en(false);
    s_ref_80m_committed_sel = -1;
    return ESP_OK;
}

// Allowed upstreams for PLL_F50M, in preference order (used by both auto-pick
// and explicit-upstream paths). Mux selectors match the HP_SYS_CLKRST
// `reg_ref_500m_sel` field: 0 = CPLL, 1 = MPLL. (shared by PLL_F25M and PLL_F50M).
static const esp_clk_tree_derived_upstream_t s_ref_500m_upstreams[] = {
    { SOC_MOD_CLK_CPLL, 0 },
    { SOC_MOD_CLK_MPLL, 1 },
};

static const esp_clk_tree_derived_clk_desc_t s_derived_pll_descs[ESP_CLK_TREE_DERIVED_PLL_NUM] = {
    [ESP_CLK_TREE_DERIVED_PLL_F25M] = {
        .clk_id         = SOC_MOD_CLK_PLL_F25M,
        .set_src        = esp_clk_tree_ref_500m_set_src,
        .set_divider    = clk_ll_pll_f25m_set_divider,
        .set_gate       = _clk_gate_ll_ref_25m_clk_en,
        .upstreams      = s_ref_500m_upstreams,
        .upstream_count = sizeof(s_ref_500m_upstreams) / sizeof(s_ref_500m_upstreams[0]),
        .state          = &s_derived_pll_states[ESP_CLK_TREE_DERIVED_PLL_F25M],
        .acquire_parent = esp_clk_tree_ref_500m_derived_acquire_parent,
        .release_parent = esp_clk_tree_ref_500m_derived_release_parent,
    },
    [ESP_CLK_TREE_DERIVED_PLL_F50M] = {
        .clk_id         = SOC_MOD_CLK_PLL_F50M,
        .set_src        = esp_clk_tree_ref_500m_set_src,
        .set_divider    = clk_ll_pll_f50m_set_divider,
        .set_gate       = _clk_gate_ll_ref_50m_clk_en,
        .upstreams      = s_ref_500m_upstreams,
        .upstream_count = sizeof(s_ref_500m_upstreams) / sizeof(s_ref_500m_upstreams[0]),
        .state          = &s_derived_pll_states[ESP_CLK_TREE_DERIVED_PLL_F50M],
        .acquire_parent = esp_clk_tree_ref_500m_derived_acquire_parent,
        .release_parent = esp_clk_tree_ref_500m_derived_release_parent,
    },
    [ESP_CLK_TREE_DERIVED_REF_F80M] = {
        .clk_id         = SOC_MOD_CLK_REF_F80M,
        .set_src        = NULL,
        .set_divider    = NULL,
        .set_gate       = _clk_gate_ll_ref_80m_clk_en,
        .upstreams      = NULL,
        .upstream_count = 0,
        .state          = &s_derived_pll_states[ESP_CLK_TREE_DERIVED_REF_F80M],
        .acquire_parent = esp_clk_tree_ref_80m_derived_acquire_parent,
        .release_parent = esp_clk_tree_ref_80m_derived_release_parent,
    },
};

const esp_clk_tree_derived_clk_desc_t *esp_clk_tree_get_derived_clk_desc(soc_module_clk_t clk_src)
{
    switch (clk_src) {
    case SOC_MOD_CLK_PLL_F25M:
        return &s_derived_pll_descs[ESP_CLK_TREE_DERIVED_PLL_F25M];
    case SOC_MOD_CLK_PLL_F50M:
        return &s_derived_pll_descs[ESP_CLK_TREE_DERIVED_PLL_F50M];
    case SOC_MOD_CLK_REF_F80M:
        return &s_derived_pll_descs[ESP_CLK_TREE_DERIVED_REF_F80M];
    default:
        return NULL;
    }
}

/* -------------------------------------------------------------------------- */
/* Fixed ref clocks: gate + static BBPLL parent only                            */
/* -------------------------------------------------------------------------- */

typedef void (*esp_clk_tree_gate_fn_t)(bool enable);
typedef void (*esp_clk_tree_parent_fn_t)(bool enable);

typedef struct {
    soc_module_clk_t          clk_id;
    esp_clk_tree_gate_fn_t    set_gate;
    esp_clk_tree_parent_fn_t  parent_power;
} esp_clk_tree_gated_clk_t;

static void esp_clk_tree_parent_bbpll(bool enable)
{
    esp_clk_tree_enable_power(SOC_ROOT_CIRCUIT_CLK_BBPLL, enable);
}

static void esp_clk_tree_parent_rc_fast(bool enable)
{
    if (enable) {
        rtc_dig_clk8m_enable();
    } else {
        rtc_dig_clk8m_disable();
    }
}

DEFINE_CRIT_SECTION_LOCK_STATIC(s_clk_tree_spinlock);

/** Per soc_module_clk_t: record clock gate consumers */
static int16_t s_mod_clk_gate_ref_cnt[SOC_MOD_CLK_INVALID] = { 0 };

/** Per soc_root_clk_circuit_t: record clock power consumers */
static int16_t s_root_pll_power_ref_cnt[SOC_ROOT_CIRCUIT_CLK_MAX] = { 0 };

static bool s_clk_tree_initialized = false;

static int16_t esp_clk_tree_root_pll_power_acquire(soc_root_clk_circuit_t clk_circuit)
{
    int16_t prev;

    assert(clk_circuit == SOC_ROOT_CIRCUIT_CLK_BBPLL || clk_circuit == SOC_ROOT_CIRCUIT_CLK_CPLL
           || clk_circuit == SOC_ROOT_CIRCUIT_CLK_XTAL_X2);

    esp_os_enter_critical(&s_clk_tree_spinlock);
    prev = s_root_pll_power_ref_cnt[clk_circuit]++;
    if (prev == 0) {
        switch (clk_circuit) {
        case SOC_ROOT_CIRCUIT_CLK_BBPLL:
            clk_ll_bbpll_enable();
            break;
        case SOC_ROOT_CIRCUIT_CLK_CPLL:
            clk_ll_cpll_enable();
            break;
        case SOC_ROOT_CIRCUIT_CLK_XTAL_X2:
            clk_ll_xtalx2_enable();
            break;
        default:
            break;
        }
    }
    esp_os_exit_critical(&s_clk_tree_spinlock);
    return prev;
}

static int16_t esp_clk_tree_root_pll_power_release(soc_root_clk_circuit_t clk_circuit)
{
    int16_t prev;

    assert(clk_circuit == SOC_ROOT_CIRCUIT_CLK_BBPLL || clk_circuit == SOC_ROOT_CIRCUIT_CLK_CPLL
           || clk_circuit == SOC_ROOT_CIRCUIT_CLK_XTAL_X2);

    esp_os_enter_critical(&s_clk_tree_spinlock);
    prev = s_root_pll_power_ref_cnt[clk_circuit];
    if (prev <= 0) {
        esp_os_exit_critical(&s_clk_tree_spinlock);
        ESP_EARLY_LOGW(TAG, "soc_root_clk_circuit_t %d disabled multiple times!!", clk_circuit);
        return prev;
    }
    s_root_pll_power_ref_cnt[clk_circuit] = prev - 1;
    if (prev == 1) {
        switch (clk_circuit) {
        case SOC_ROOT_CIRCUIT_CLK_BBPLL:
            clk_ll_bbpll_disable();
            break;
        case SOC_ROOT_CIRCUIT_CLK_CPLL:
            clk_ll_cpll_disable();
            break;
        case SOC_ROOT_CIRCUIT_CLK_XTAL_X2:
            clk_ll_xtalx2_disable();
            break;
        default:
            break;
        }
    }
    esp_os_exit_critical(&s_clk_tree_spinlock);
    return prev;
}

static uint32_t esp_clk_tree_ref_500m_pll_get_freq_hz(uint32_t div_num)
{
    uint32_t up_hz;

    if (clk_ll_ref_500m_get_src() == 0) {
        up_hz = clk_ll_cpll_get_freq_mhz(clk_hal_xtal_get_freq_mhz()) * MHZ;
    } else {
        up_hz = clk_ll_mpll_get_freq_mhz(clk_hal_xtal_get_freq_mhz()) * MHZ;
    }
    return up_hz / div_num;
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
    case SOC_MOD_CLK_PLL_F25M:
        clk_src_freq = esp_clk_tree_ref_500m_pll_get_freq_hz(clk_ll_pll_f25m_get_divider());
        break;
    case SOC_MOD_CLK_PLL_F50M:
        clk_src_freq = esp_clk_tree_ref_500m_pll_get_freq_hz(clk_ll_pll_f50m_get_divider());
        break;
    case SOC_MOD_CLK_PLL_F60M:
        clk_src_freq = CLK_LL_PLL_60M_FREQ_MHZ * MHZ;
        break;
    case SOC_MOD_CLK_REF_F80M:
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

void esp_clk_tree_initialize(void)
{
    soc_reset_reason_t rst_reason = esp_rom_get_reset_reason(0);
    soc_periph_flash_clk_src_t flash_clk_src = _mspi_timing_ll_get_flash_clk_src(MSPI_TIMING_LL_MSPI_ID_0);
    soc_cpu_clk_src_t cpu_src = clk_ll_cpu_get_src();
    bool cpu_reset = (rst_reason == RESET_REASON_CPU_SW) || (rst_reason == RESET_REASON_CPU_MWDT) ||
                     (rst_reason == RESET_REASON_CPU_RWDT) || (rst_reason == RESET_REASON_CPU_JTAG) ||
                     (rst_reason == RESET_REASON_CPU_LOCKUP);
    if (!cpu_reset) {
        /* Cold boot only: gate / power-down clocks not in use. */
        if (cpu_src != SOC_CPU_CLK_SRC_PLL_F240M) {
            _clk_gate_ll_ref_240m_clk_en(false);
        }
        if (cpu_src != SOC_CPU_CLK_SRC_CPLL && flash_clk_src != FLASH_CLK_SRC_CPLL) {
            clk_ll_cpll_disable();
        }
        _clk_gate_ll_ref_160m_clk_en(false);
        _clk_gate_ll_ref_120m_clk_en(false);
        _clk_gate_ll_ref_80m_clk_en(false);
        _clk_gate_ll_ref_60m_clk_en(false);
        _clk_gate_ll_ref_20m_clk_en(false);
        _clk_gate_ll_ref_50m_clk_en(false);
        _clk_gate_ll_ref_25m_clk_en(false);
        clk_ll_xtalx2_disable();
        HP_ALIVE_SYS.hp_clk_ctrl.hp_audio_pll_clk_en = 0;
        HP_ALIVE_SYS.hp_clk_ctrl.hp_sdio_pll2_clk_en = 0;
        HP_ALIVE_SYS.hp_clk_ctrl.hp_sdio_pll1_clk_en = 0;
        HP_ALIVE_SYS.hp_clk_ctrl.hp_sdio_pll0_clk_en = 0;
    }

    s_clk_tree_initialized = true;
#if CONFIG_USJ_ENABLE_USB_SERIAL_JTAG || CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED
    /* Bootloader / USJ may keep BBPLL 480M on; declare a permanent hold. */
    esp_clk_tree_enable_src(SOC_MOD_CLK_BBPLL, true);
#endif
    /* Flash + CPU: sync clk_tree refs with HW already selected at boot. */
    esp_clk_tree_enable_src((soc_module_clk_t)flash_clk_src, true);
    if (cpu_src == SOC_CPU_CLK_SRC_CPLL) {
        esp_clk_tree_enable_src(SOC_MOD_CLK_CPLL, true);
    } else if (cpu_src == SOC_CPU_CLK_SRC_PLL_F240M) {
        esp_clk_tree_enable_src(SOC_MOD_CLK_PLL_F240M, true);
    }
}

bool esp_clk_tree_enable_power(soc_root_clk_circuit_t clk_circuit, bool enable)
{
    if (clk_circuit >= SOC_ROOT_CIRCUIT_CLK_MAX) {
        return false;
    }

    bool toggled = false;
    switch (clk_circuit) {
    case SOC_ROOT_CIRCUIT_CLK_CPLL:
    case SOC_ROOT_CIRCUIT_CLK_BBPLL:
    case SOC_ROOT_CIRCUIT_CLK_XTAL_X2: {
        if (enable) {
            toggled = (esp_clk_tree_root_pll_power_acquire(clk_circuit) == 0);
        } else {
            toggled = (esp_clk_tree_root_pll_power_release(clk_circuit) == 1);
        }
        break;
    }
    default:
        break;
    }
    return toggled;
}

bool esp_clk_tree_port_is_power_on(soc_root_clk_circuit_t clk_circuit)
{
    if (clk_circuit >= SOC_ROOT_CIRCUIT_CLK_MAX) {
        return false;
    }
    if (clk_circuit == SOC_ROOT_CIRCUIT_CLK_BBPLL || clk_circuit == SOC_ROOT_CIRCUIT_CLK_CPLL
            || clk_circuit == SOC_ROOT_CIRCUIT_CLK_XTAL_X2) {
        int16_t cnt;

        esp_os_enter_critical(&s_clk_tree_spinlock);
        cnt = s_root_pll_power_ref_cnt[clk_circuit];
        esp_os_exit_critical(&s_clk_tree_spinlock);
        return cnt > 0;
    }
    return false;
}

typedef enum {
    ESP_CLK_TREE_GATED_CLK_RC_FAST,
    ESP_CLK_TREE_GATED_CLK_PLL_F20M,
    ESP_CLK_TREE_GATED_CLK_PLL_F60M,
    ESP_CLK_TREE_GATED_CLK_PLL_F120M,
    ESP_CLK_TREE_GATED_CLK_PLL_F160M,
    ESP_CLK_TREE_GATED_CLK_PLL_F240M,
    ESP_CLK_TREE_GATED_CLK_NUM,
} esp_clk_tree_gated_clk_id_t;

static const esp_clk_tree_gated_clk_t s_gated_ref_clks[] = {
    [ESP_CLK_TREE_GATED_CLK_RC_FAST]    = { SOC_MOD_CLK_RC_FAST,      NULL,                           esp_clk_tree_parent_rc_fast },
    [ESP_CLK_TREE_GATED_CLK_PLL_F20M]   = { SOC_MOD_CLK_PLL_F20M,     _clk_gate_ll_ref_20m_clk_en,     esp_clk_tree_parent_bbpll },
    [ESP_CLK_TREE_GATED_CLK_PLL_F60M]   = { SOC_MOD_CLK_PLL_F60M,     _clk_gate_ll_ref_60m_clk_en,     esp_clk_tree_parent_bbpll },
    [ESP_CLK_TREE_GATED_CLK_PLL_F120M]  = { SOC_MOD_CLK_PLL_F120M,    _clk_gate_ll_ref_120m_clk_en,    esp_clk_tree_parent_bbpll },
    [ESP_CLK_TREE_GATED_CLK_PLL_F160M]  = { SOC_MOD_CLK_PLL_F160M,    _clk_gate_ll_ref_160m_clk_en,    esp_clk_tree_parent_bbpll },
    [ESP_CLK_TREE_GATED_CLK_PLL_F240M]  = { SOC_MOD_CLK_PLL_F240M,    _clk_gate_ll_ref_240m_clk_en,    esp_clk_tree_parent_bbpll },
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
    bool released_too_many = false;

    esp_os_enter_critical(&s_clk_tree_spinlock);
    if (enable) {
        prev_ref_cnt = s_mod_clk_gate_ref_cnt[entry->clk_id]++;
        if (prev_ref_cnt == 0) {
            if (entry->parent_power != NULL) {
                entry->parent_power(true);
            }
            ENABLE_CLK_GATE(entry->set_gate, true);
        }
    } else {
        prev_ref_cnt = s_mod_clk_gate_ref_cnt[entry->clk_id]--;
        if (prev_ref_cnt <= 0) {
            s_mod_clk_gate_ref_cnt[entry->clk_id] = 0;
            released_too_many = true;
        } else if (prev_ref_cnt == 1) {
            ENABLE_CLK_GATE(entry->set_gate, false);
            if (entry->parent_power != NULL) {
                entry->parent_power(false);
            }
        }
    }
    esp_os_exit_critical(&s_clk_tree_spinlock);

    if (released_too_many) {
        ESP_LOGW(TAG, "soc_module_clk_t %d disabled multiple times!!", entry->clk_id);
    }
    return ESP_OK;
}

esp_err_t esp_clk_tree_enable_src(soc_module_clk_t clk_src, bool enable)
{
    if (clk_src < 1 || clk_src >= SOC_MOD_CLK_INVALID || clk_src == SOC_MOD_CLK_XTAL) {
        /* Not managed by esp_clk_tree*/
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
    case SOC_MOD_CLK_BBPLL:
        esp_clk_tree_enable_power(SOC_ROOT_CIRCUIT_CLK_BBPLL, enable);
        return ESP_OK;
    case SOC_MOD_CLK_CPLL:
        esp_clk_tree_enable_power(SOC_ROOT_CIRCUIT_CLK_CPLL, enable);
        return ESP_OK;
    case SOC_MOD_CLK_RC_FAST:   gated_clk_id = ESP_CLK_TREE_GATED_CLK_RC_FAST;   break;
    case SOC_MOD_CLK_PLL_F20M:  gated_clk_id = ESP_CLK_TREE_GATED_CLK_PLL_F20M;  break;
    case SOC_MOD_CLK_PLL_F60M:  gated_clk_id = ESP_CLK_TREE_GATED_CLK_PLL_F60M;  break;
    case SOC_MOD_CLK_PLL_F120M: gated_clk_id = ESP_CLK_TREE_GATED_CLK_PLL_F120M; break;
    case SOC_MOD_CLK_PLL_F160M: gated_clk_id = ESP_CLK_TREE_GATED_CLK_PLL_F160M; break;
    case SOC_MOD_CLK_PLL_F240M: gated_clk_id = ESP_CLK_TREE_GATED_CLK_PLL_F240M; break;
    default:
        // Derived PLL clocks (PLL_F25M/F50M/F80M) use the shared derived-clk engine.
        if (esp_clk_tree_get_derived_clk_desc(clk_src) != NULL) {
            return enable ? esp_clk_tree_derived_clk_acquire(clk_src)
                          : esp_clk_tree_derived_clk_release(clk_src);
        }
        return ESP_OK;
    }
    return esp_clk_tree_enable_gated_clk(&s_gated_ref_clks[gated_clk_id], enable);
}
