/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
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
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/critical_section.h"

ESP_LOG_ATTR_TAG(TAG, "esp_clk_tree");

/* -------------------------------------------------------------------------- */
/* Fixed ref clocks: gate + static parent power                                */
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

static void esp_clk_tree_parent_xtal_x2(bool enable)
{
    esp_clk_tree_enable_power(SOC_ROOT_CIRCUIT_CLK_XTAL_X2, enable);
}

// static void esp_clk_tree_parent_rc_fast(bool enable)
// {
//     if (enable) {
//         rtc_dig_clk8m_enable();
//     } else {
//         rtc_dig_clk8m_disable();
//     }
// }

DEFINE_CRIT_SECTION_LOCK_STATIC(s_clk_tree_spinlock);

/** Per soc_module_clk_t: record clock gate consumers */
static int16_t s_mod_clk_gate_ref_cnt[SOC_MOD_CLK_INVALID] = { 0 };

/** Per soc_root_clk_circuit_t: record clock power consumers */
static int16_t s_root_pll_power_ref_cnt[SOC_ROOT_CIRCUIT_CLK_MAX] = { 0 };

static bool s_clk_tree_initialized = false;

static int16_t esp_clk_tree_root_pll_power_acquire(soc_root_clk_circuit_t clk_circuit)
{
    int16_t prev;

    assert(clk_circuit == SOC_ROOT_CIRCUIT_CLK_BBPLL || clk_circuit == SOC_ROOT_CIRCUIT_CLK_XTAL_X2);

    esp_os_enter_critical(&s_clk_tree_spinlock);
    prev = s_root_pll_power_ref_cnt[clk_circuit]++;
    if (prev == 0) {
        switch (clk_circuit) {
        case SOC_ROOT_CIRCUIT_CLK_BBPLL:
            clk_ll_bbpll_enable();
            break;
        case SOC_ROOT_CIRCUIT_CLK_XTAL_X2:
            clk_ll_xtal_x2_enable();
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

    assert(clk_circuit == SOC_ROOT_CIRCUIT_CLK_BBPLL || clk_circuit == SOC_ROOT_CIRCUIT_CLK_XTAL_X2);

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
        case SOC_ROOT_CIRCUIT_CLK_XTAL_X2:
            clk_ll_xtal_x2_disable();
            break;
        default:
            break;
        }
    }
    esp_os_exit_critical(&s_clk_tree_spinlock);
    return prev;
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
    case SOC_MOD_CLK_PLL_F48M:
        clk_src_freq = CLK_LL_PLL_48M_FREQ_MHZ * MHZ;
        break;
    case SOC_MOD_CLK_XTAL_X2:
    case SOC_MOD_CLK_XTAL_X2_F64M:
        clk_src_freq = CLK_LL_PLL_64M_FREQ_MHZ * MHZ;
        break;
    case SOC_MOD_CLK_BBPLL:
    case SOC_MOD_CLK_PLL_F96M:
        clk_src_freq = CLK_LL_PLL_96M_FREQ_MHZ * MHZ;
        break;
    case SOC_MOD_CLK_RTC_SLOW:
        clk_src_freq = esp_clk_tree_lp_slow_get_freq_hz(precision);
        break;
    case SOC_MOD_CLK_RTC_FAST:
        clk_src_freq = esp_clk_tree_lp_fast_get_freq_hz(precision);
        break;
    case SOC_MOD_CLK_RC_FAST:
        clk_src_freq = esp_clk_tree_rc_fast_get_freq_hz(precision);
        break;
    case SOC_MOD_CLK_XTAL32K:
        clk_src_freq = esp_clk_tree_xtal32k_get_freq_hz(precision);
        break;
    default:
        break;
    }

    ESP_RETURN_ON_FALSE(clk_src_freq, ESP_FAIL, TAG, "freq shouldn't be 0, calibration failed");
    *freq_value = clk_src_freq;
    return ESP_OK;
}

esp_err_t esp_clk_tree_src_set_freq_hz(soc_module_clk_t clk_src, uint32_t expt_freq_value, uint32_t *ret_freq_value)
{
    ESP_RETURN_ON_FALSE(clk_src > 0 && clk_src < SOC_MOD_CLK_INVALID, ESP_ERR_INVALID_ARG, TAG, "unknown clk src");
    ESP_RETURN_ON_FALSE(expt_freq_value > 0, ESP_ERR_INVALID_ARG, TAG, "invalid frequency");

    (void)ret_freq_value;
    return ESP_ERR_NOT_SUPPORTED;
}

void esp_clk_tree_initialize(void)
{
    soc_reset_reason_t rst_reason = esp_rom_get_reset_reason(0);
    soc_cpu_clk_src_t cpu_src = clk_ll_cpu_get_src();
    bool cpu_reset = (rst_reason == RESET_REASON_CPU0_MWDT0) || (rst_reason == RESET_REASON_CPU0_MWDT1) ||
                     (rst_reason == RESET_REASON_CPU0_SW) || (rst_reason == RESET_REASON_CPU0_RTC_WDT) ||
                     (rst_reason == RESET_REASON_CPU0_JTAG);
    if (!cpu_reset) {
        /* Cold boot only: gate / power-down clocks not in use.
         * Flash MSPI defaults to XTAL_X2_F64M — keep that root/gate alive. */
        _clk_gate_ll_ref_8m_clk_en(false);
        _clk_gate_ll_ref_16m_clk_en(false);
        _clk_gate_ll_ref_32m_clk_en(false);
        _clk_gate_ll_ref_96m_clk_en(false);
#if CONFIG_USJ_ENABLE_USB_SERIAL_JTAG || CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED
        /* Keep BBPLL / ref_48m: USJ PHY still uses them after bootloader. */
#else
        _clk_gate_ll_ref_48m_clk_en(false);
        if (cpu_src != SOC_CPU_CLK_SRC_PLL) {
            clk_ll_bbpll_disable();
        }
#endif
    }

    s_clk_tree_initialized = true;
#if CONFIG_USJ_ENABLE_USB_SERIAL_JTAG || CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED
    /* Bootloader / USJ may keep PLL_F48M on; declare a permanent hold. */
    esp_clk_tree_enable_src(SOC_MOD_CLK_PLL_F48M, true);
#endif
    /* Flash + CPU: sync clk_tree refs with HW already selected at boot.
     * Flash uses gated F64M; CPU holds the ungated XTAL_X2 / PLL root, not the F64M / F96M gates. */
    esp_clk_tree_enable_src(SOC_MOD_CLK_XTAL_X2_F64M, true);
    if (cpu_src == SOC_CPU_CLK_SRC_PLL) {
        esp_clk_tree_enable_src(SOC_MOD_CLK_BBPLL, true);
    } else if (cpu_src == SOC_CPU_CLK_SRC_XTAL_X2) {
        esp_clk_tree_enable_src(SOC_MOD_CLK_XTAL_X2, true);
    }
}

bool esp_clk_tree_enable_power(soc_root_clk_circuit_t clk_circuit, bool enable)
{
    if (clk_circuit >= SOC_ROOT_CIRCUIT_CLK_MAX) {
        return false;
    }

    bool toggled = false;
    switch (clk_circuit) {
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
    if (clk_circuit == SOC_ROOT_CIRCUIT_CLK_BBPLL || clk_circuit == SOC_ROOT_CIRCUIT_CLK_XTAL_X2) {
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
    ESP_CLK_TREE_GATED_CLK_PLL_F48M,
    ESP_CLK_TREE_GATED_CLK_XTAL_X2_F64M,
    ESP_CLK_TREE_GATED_CLK_PLL_F96M,
    ESP_CLK_TREE_GATED_CLK_NUM,
} esp_clk_tree_gated_clk_id_t;

static const esp_clk_tree_gated_clk_t s_gated_ref_clks[] = {
    // [ESP_CLK_TREE_GATED_CLK_RC_FAST]       = { SOC_MOD_CLK_RC_FAST,       NULL,                        esp_clk_tree_parent_rc_fast }, // TODO: PM-859
    [ESP_CLK_TREE_GATED_CLK_PLL_F48M]      = { SOC_MOD_CLK_PLL_F48M,      _clk_gate_ll_ref_48m_clk_en, esp_clk_tree_parent_bbpll },
    [ESP_CLK_TREE_GATED_CLK_XTAL_X2_F64M]  = { SOC_MOD_CLK_XTAL_X2_F64M,  _clk_gate_ll_ref_64m_clk_en, esp_clk_tree_parent_xtal_x2 },
    [ESP_CLK_TREE_GATED_CLK_PLL_F96M]      = { SOC_MOD_CLK_PLL_F96M,      _clk_gate_ll_ref_96m_clk_en, esp_clk_tree_parent_bbpll },
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
        /* Not managed by esp_clk_tree */
        return ESP_OK;
    }

    if (!s_clk_tree_initialized) {
        return ESP_OK;
    }

    esp_clk_tree_gated_clk_id_t gated_clk_id;
    switch (clk_src) {
    case SOC_MOD_CLK_XTAL_X2:
        esp_clk_tree_enable_power(SOC_ROOT_CIRCUIT_CLK_XTAL_X2, enable);
        return ESP_OK;
    case SOC_MOD_CLK_BBPLL:
        esp_clk_tree_enable_power(SOC_ROOT_CIRCUIT_CLK_BBPLL, enable);
        return ESP_OK;
    // case SOC_MOD_CLK_RC_FAST:       gated_clk_id = ESP_CLK_TREE_GATED_CLK_RC_FAST;       break;
    case SOC_MOD_CLK_PLL_F48M:      gated_clk_id = ESP_CLK_TREE_GATED_CLK_PLL_F48M;      break;
    case SOC_MOD_CLK_XTAL_X2_F64M:  gated_clk_id = ESP_CLK_TREE_GATED_CLK_XTAL_X2_F64M;  break;
    case SOC_MOD_CLK_PLL_F96M:      gated_clk_id = ESP_CLK_TREE_GATED_CLK_PLL_F96M;      break;
    default:
        return ESP_OK;
    }
    return esp_clk_tree_enable_gated_clk(&s_gated_ref_clks[gated_clk_id], enable);
}
