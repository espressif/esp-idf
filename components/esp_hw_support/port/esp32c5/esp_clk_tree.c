/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "esp_clk_tree.h"
#include "esp_err.h"
#include "esp_check.h"
#include "soc/rtc.h"
#include "hal/clk_tree_hal.h"
#include "hal/clk_tree_ll.h"
#include "hal/clk_gate_ll.h"
#include "esp_private/esp_clk_tree_common.h"

static const char *TAG = "esp_clk_tree";

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

#define ENUM2ARRAY(clk_src) (clk_src - SOC_MOD_CLK_PLL_F12M)
static __NOINIT_ATTR int16_t s_pll_src_cg_ref_cnt[9] = { 0 };
static bool esp_clk_tree_initialized = false;

void esp_clk_tree_initialize(void)
{
    soc_reset_reason_t rst_reason = esp_rom_get_reset_reason(0);
    if ((rst_reason == RESET_REASON_CPU0_MWDT0) || (rst_reason == RESET_REASON_CPU0_MWDT1)          \
            || (rst_reason == RESET_REASON_CPU0_SW) || (rst_reason == RESET_REASON_CPU0_RTC_WDT)    \
            || (rst_reason == RESET_REASON_CPU0_JTAG) || (rst_reason == RESET_REASON_CPU0_LOCKUP)) {
        esp_clk_tree_initialized = true;
        return;
    } else {
        bzero(s_pll_src_cg_ref_cnt, sizeof(s_pll_src_cg_ref_cnt));
    }

    soc_cpu_clk_src_t current_cpu_clk_src = clk_ll_cpu_get_src();

    if (current_cpu_clk_src == SOC_CPU_CLK_SRC_PLL_F160M) {
        s_pll_src_cg_ref_cnt[ENUM2ARRAY(SOC_MOD_CLK_PLL_F160M)] = 1;
        _clk_gate_ll_ref_240m_clk_en(false);
    } else if (current_cpu_clk_src == SOC_CPU_CLK_SRC_PLL_F240M) {
        s_pll_src_cg_ref_cnt[ENUM2ARRAY(SOC_MOD_CLK_PLL_F240M)] = 1;
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

esp_err_t esp_clk_tree_enable_src(soc_module_clk_t clk_src, bool enable)
{
    if (!esp_clk_tree_initialized || (clk_src < SOC_MOD_CLK_PLL_F12M) || (clk_src > SOC_MOD_CLK_PLL_F240M)) {
        return ESP_OK;
    }

    PERIPH_RCC_ATOMIC() {
        if (enable) {
            s_pll_src_cg_ref_cnt[ENUM2ARRAY(clk_src)]++;
        }
        if (s_pll_src_cg_ref_cnt[ENUM2ARRAY(clk_src)] == 1) {
            switch (clk_src) {
                case SOC_MOD_CLK_PLL_F12M:  clk_gate_ll_ref_12m_clk_en(enable);  break;
                case SOC_MOD_CLK_PLL_F20M:  clk_gate_ll_ref_20m_clk_en(enable);  break;
                case SOC_MOD_CLK_PLL_F40M:  clk_gate_ll_ref_40m_clk_en(enable);  break;
                case SOC_MOD_CLK_PLL_F48M:  clk_gate_ll_ref_48m_clk_en(enable);  break;
                case SOC_MOD_CLK_PLL_F60M:  clk_gate_ll_ref_60m_clk_en(enable);  break;
                case SOC_MOD_CLK_PLL_F80M:  clk_gate_ll_ref_80m_clk_en(enable);  break;
                case SOC_MOD_CLK_PLL_F120M: clk_gate_ll_ref_120m_clk_en(enable); break;
                case SOC_MOD_CLK_PLL_F160M: clk_gate_ll_ref_160m_clk_en(enable); break;
                case SOC_MOD_CLK_PLL_F240M: clk_gate_ll_ref_240m_clk_en(enable); break;
                default: break;
            }
        }
        if (!enable) {
            s_pll_src_cg_ref_cnt[ENUM2ARRAY(clk_src)]--;
        }
        assert(s_pll_src_cg_ref_cnt[ENUM2ARRAY(clk_src)] >= 0);
    }
    return ESP_OK;
}
#undef ENUM2ARRAY
