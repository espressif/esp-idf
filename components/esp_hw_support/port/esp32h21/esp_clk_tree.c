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

//TODO: [ESP32H21] IDF-11521

static const char *TAG = "esp_clk_tree";

esp_err_t esp_clk_tree_src_get_freq_hz(soc_module_clk_t clk_src, esp_clk_tree_src_freq_precision_t precision,
uint32_t *freq_value)
{
    ESP_RETURN_ON_FALSE(clk_src > 0 && clk_src < SOC_MOD_CLK_INVALID, ESP_ERR_INVALID_ARG, TAG, "unknown clk src");
    ESP_RETURN_ON_FALSE(precision < ESP_CLK_TREE_SRC_FREQ_PRECISION_INVALID, ESP_ERR_INVALID_ARG, TAG, "unknown precision");
    ESP_RETURN_ON_FALSE(freq_value, ESP_ERR_INVALID_ARG, TAG, "null pointer");

#if !SOC_CLK_TREE_SUPPORTED
    // Have only XTAL 32M before clock tree supported
    assert(clk_src == SOC_MOD_CLK_XTAL);
#endif
    uint32_t clk_src_freq = 0;
    switch (clk_src) {
    case SOC_MOD_CLK_XTAL:
        clk_src_freq = SOC_XTAL_FREQ_32M * MHZ;
        break;
    case SOC_MOD_CLK_PLL_F48M:
        clk_src_freq = CLK_LL_PLL_48M_FREQ_MHZ * MHZ;
        break;
    case SOC_MOD_CLK_PLL_F64M:
        clk_src_freq = CLK_LL_PLL_64M_FREQ_MHZ * MHZ;
        break;
    case SOC_MOD_CLK_PLL_F96M:
        clk_src_freq = CLK_LL_PLL_96M_FREQ_MHZ * MHZ;
        break;
    default:
        break;
    }

    ESP_RETURN_ON_FALSE(clk_src_freq, ESP_FAIL, TAG, "freq shouldn't be 0, calibration failed");
    *freq_value = clk_src_freq;
    return ESP_OK;
}

esp_err_t esp_clk_tree_enable_src(soc_module_clk_t clk_src, bool enable)
{
    (void)clk_src; (void)enable;
    return ESP_OK;
}
