/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_clk_tree.h"
#include "esp_err.h"
#include "esp_check.h"
#include "soc/rtc.h"
#include "hal/clk_tree_hal.h"
#include "hal/clk_tree_ll.h"
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
    case SOC_MOD_CLK_PLL_F48M:
        clk_src_freq = CLK_LL_PLL_48M_FREQ_MHZ * MHZ;
        break;
    case SOC_MOD_CLK_XTAL_X2_F64M:
        clk_src_freq = CLK_LL_PLL_64M_FREQ_MHZ * MHZ;
        break;
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

static int16_t s_xtal_x2_ref_cnt = 0;

void esp_clk_tree_initialize(void)
{
    // In bootloader, flash clock source will always be switched to use XTAL_X2 clock
    s_xtal_x2_ref_cnt++;
    if (clk_ll_cpu_get_src() == SOC_CPU_CLK_SRC_XTAL_X2) {
        s_xtal_x2_ref_cnt++;
    }
}

bool esp_clk_tree_is_power_on(soc_root_clk_circuit_t clk_circuit)
{
    switch (clk_circuit) {
    case SOC_ROOT_CIRCUIT_CLK_XTAL_X2:
        return s_xtal_x2_ref_cnt > 0;
    default:
        break;
    }
    return false;
}

esp_err_t esp_clk_tree_enable_power(soc_root_clk_circuit_t clk_circuit, bool enable)
{
    switch (clk_circuit) {
    case SOC_ROOT_CIRCUIT_CLK_XTAL_X2:
        if (enable) {
            s_xtal_x2_ref_cnt++;
        } else {
            s_xtal_x2_ref_cnt--;
        }

        if (s_xtal_x2_ref_cnt == 1) {
            clk_ll_xtal_x2_enable();
        } else if (s_xtal_x2_ref_cnt == 0) {
            clk_ll_xtal_x2_disable();
        }

        assert(s_xtal_x2_ref_cnt >= 0);
        break;
    default:
        break;
    }
    return ESP_OK; // TODO: PM-354
}

esp_err_t esp_clk_tree_enable_src(soc_module_clk_t clk_src, bool enable)
{
    switch (clk_src) {
    case SOC_MOD_CLK_XTAL_X2_F64M:
        //  later, here should handle ref count for PLL_F64M clock gating, then also handle XTAL_X2 circuit enable/disable
        esp_clk_tree_enable_power(SOC_ROOT_CIRCUIT_CLK_XTAL_X2, enable);
    default:
        break;
    }
    return ESP_OK; // TODO: PM-354
}
