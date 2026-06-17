/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
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
#include "soc/rtc.h"
#include "hal/clk_tree_hal.h"
#include "hal/clk_tree_ll.h"
#include "hal/clk_gate_ll.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/periph_ctrl.h"

ESP_LOG_ATTR_TAG(TAG, "esp_clk_tree");

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
    case SOC_MOD_CLK_PLL_F80M:
        clk_src_freq = CLK_LL_PLL_80M_FREQ_MHZ * MHZ;
        break;
    case SOC_MOD_CLK_PLL_F160M:
        clk_src_freq = CLK_LL_PLL_160M_FREQ_MHZ * MHZ;
        break;
    case SOC_MOD_CLK_PLL_F240M:
        clk_src_freq = CLK_LL_PLL_240M_FREQ_MHZ * MHZ;
        break;
    case SOC_MOD_CLK_SPLL:
        clk_src_freq = CLK_LL_PLL_480M_FREQ_MHZ * MHZ;
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
    case SOC_MOD_CLK_XTAL_D2:
        clk_src_freq = (clk_hal_xtal_get_freq_mhz() * MHZ) >> 1;
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
    (void)clk_src; (void)expt_freq_value; (void)ret_freq_value;
    return ESP_ERR_NOT_SUPPORTED;
}

static _Atomic int16_t s_pll_src_cg_ref_cnt[SOC_MOD_CLK_INVALID] = { 0 };
static bool esp_clk_tree_initialized = false;

void esp_clk_tree_initialize(void)
{
    soc_reset_reason_t rst_reason = esp_rom_get_reset_reason(0);
    if ((rst_reason == RESET_REASON_CPU0_MWDT0) || (rst_reason == RESET_REASON_CPU0_MWDT1)          \
            || (rst_reason == RESET_REASON_CPU0_SW) || (rst_reason == RESET_REASON_CPU0_RTC_WDT)    \
            || (rst_reason == RESET_REASON_CPU0_JTAG) || (rst_reason == RESET_REASON_CPU0_LOCKUP)) {
        esp_clk_tree_initialized = true;
        return;
    }

    soc_cpu_clk_src_t current_cpu_clk_src = clk_ll_cpu_get_src();

    if (current_cpu_clk_src == SOC_CPU_CLK_SRC_PLL_F160M) {
        s_pll_src_cg_ref_cnt[SOC_MOD_CLK_PLL_F160M] = 1;
        _clk_gate_ll_ref_240m_clk_en(false);
    } else if (current_cpu_clk_src == SOC_CPU_CLK_SRC_PLL_F240M) {
        s_pll_src_cg_ref_cnt[SOC_MOD_CLK_PLL_F240M] = 1;
        _clk_gate_ll_ref_160m_clk_en(false);
    }
    _clk_gate_ll_ref_120m_clk_en(false);
    _clk_gate_ll_ref_80m_clk_en(false);
    _clk_gate_ll_ref_60m_clk_en(false);
#if !CONFIG_USJ_ENABLE_USB_SERIAL_JTAG && !CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED
    _clk_gate_ll_ref_48m_clk_en(false);
#endif
    _clk_gate_ll_ref_40m_clk_en(false);
    _clk_gate_ll_ref_20m_clk_en(false);
    _clk_gate_ll_ref_12m_clk_en(false);
    esp_clk_tree_initialized = true;
}

bool esp_clk_tree_is_power_on(soc_root_clk_circuit_t clk_circuit)
{
    (void)clk_circuit;
    return false;
}

bool esp_clk_tree_enable_power(soc_root_clk_circuit_t clk_circuit, bool enable)
{
    (void)clk_circuit; (void)enable;
    return false; // TODO: PM-653
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
    if (!esp_clk_tree_initialized) {
        return ESP_OK;
    }

    int16_t prev_ref_cnt = 0;
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
    if ((prev_ref_cnt == 0 && enable) || (prev_ref_cnt == 1 && !enable)) {
        switch (clk_src) {
            case SOC_MOD_CLK_RC_FAST:   enable ? rtc_dig_clk8m_enable() : rtc_dig_clk8m_disable();  break;
            case SOC_MOD_CLK_PLL_F12M:  ENABLE_CLK_GATE(clk_gate_ll_ref_12m_clk_en, enable);  break;
            case SOC_MOD_CLK_PLL_F20M:  ENABLE_CLK_GATE(clk_gate_ll_ref_20m_clk_en, enable);  break;
            case SOC_MOD_CLK_PLL_F40M:  ENABLE_CLK_GATE(clk_gate_ll_ref_40m_clk_en, enable);  break;
            case SOC_MOD_CLK_PLL_F48M:  ENABLE_CLK_GATE(clk_gate_ll_ref_48m_clk_en, enable);  break;
            case SOC_MOD_CLK_PLL_F60M:  ENABLE_CLK_GATE(clk_gate_ll_ref_60m_clk_en, enable);  break;
            case SOC_MOD_CLK_PLL_F80M:  ENABLE_CLK_GATE(clk_gate_ll_ref_80m_clk_en, enable);  break;
            case SOC_MOD_CLK_PLL_F120M: ENABLE_CLK_GATE(clk_gate_ll_ref_120m_clk_en, enable); break;
            case SOC_MOD_CLK_PLL_F160M: ENABLE_CLK_GATE(clk_gate_ll_ref_160m_clk_en, enable); break;
            case SOC_MOD_CLK_PLL_F240M: ENABLE_CLK_GATE(clk_gate_ll_ref_240m_clk_en, enable); break;
            default: break;
        }
    }

    return ESP_OK;
}
