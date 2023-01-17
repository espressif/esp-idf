/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "clk_tree.h"
#include "esp_err.h"
#include "esp_check.h"
#include "soc/rtc.h"
#include "hal/clk_tree_hal.h"
#include "hal/clk_tree_ll.h"
#include "esp_private/clk_tree_common.h"

static const char *TAG = "clk_tree";

esp_err_t clk_tree_src_get_freq_hz(soc_module_clk_t clk_src, clk_tree_src_freq_precision_t precision,
uint32_t *freq_value)
{
    ESP_RETURN_ON_FALSE(clk_src > 0 && clk_src < SOC_MOD_CLK_INVALID, ESP_ERR_INVALID_ARG, TAG, "unknown clk src");
    ESP_RETURN_ON_FALSE(precision < CLK_TREE_SRC_FREQ_PRECISION_INVALID, ESP_ERR_INVALID_ARG, TAG, "unknown precision");
    ESP_RETURN_ON_FALSE(freq_value, ESP_ERR_INVALID_ARG, TAG, "null pointer");

    uint32_t clk_src_freq = 0;
    switch (clk_src) {
    case SOC_MOD_CLK_CPU:
        clk_src_freq = clk_hal_cpu_get_freq_hz();
        break;
    case SOC_MOD_CLK_AHB:
        clk_src_freq = clk_hal_ahb_get_freq_hz();
        break;
    case SOC_MOD_CLK_APB:
        clk_src_freq = clk_hal_apb_get_freq_hz();
        break;
    case SOC_MOD_CLK_XTAL:
        clk_src_freq = clk_hal_xtal_get_freq_mhz() * MHZ;
        break;
    case SOC_MOD_CLK_PLL:
        clk_src_freq = clk_ll_bbpll_get_freq_mhz() * MHZ;
        break;
    case SOC_MOD_CLK_RTC_SLOW:
        clk_src_freq = clk_tree_lp_slow_get_freq_hz(precision);
        break;
    case SOC_MOD_CLK_RTC_FAST:
        clk_src_freq = clk_tree_lp_fast_get_freq_hz(precision);
        break;
    case SOC_MOD_CLK_RC_FAST:
        clk_src_freq = clk_tree_rc_fast_get_freq_hz(precision);
        break;
    case SOC_MOD_CLK_XTAL32K:
        clk_src_freq = clk_tree_xtal32k_get_freq_hz(precision);
        break;
    default:
        break;
    }

    ESP_RETURN_ON_FALSE(clk_src_freq, ESP_FAIL, TAG,
                        "freq shouldn't be 0, calibration failed");
    *freq_value = clk_src_freq;
    return ESP_OK;
}
