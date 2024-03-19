/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
// #include "esp_private/esp_clk_tree_common.h"
#include "sdkconfig.h"

static const char *TAG = "esp_clk_tree";

esp_err_t esp_clk_tree_src_get_freq_hz(soc_module_clk_t clk_src, esp_clk_tree_src_freq_precision_t precision,
uint32_t *freq_value)
{
    // TODO: [ESP32C5] IDF-8642 check again for MP version
    ESP_RETURN_ON_FALSE(clk_src > 0 && clk_src < SOC_MOD_CLK_INVALID, ESP_ERR_INVALID_ARG, TAG, "unknown clk src");
    ESP_RETURN_ON_FALSE(precision < ESP_CLK_TREE_SRC_FREQ_PRECISION_INVALID, ESP_ERR_INVALID_ARG, TAG, "unknown precision");
    ESP_RETURN_ON_FALSE(freq_value, ESP_ERR_INVALID_ARG, TAG, "null pointer");

    uint32_t clk_src_freq = 0;
    switch (clk_src) {
    case SOC_MOD_CLK_XTAL:
        clk_src_freq = CONFIG_XTAL_FREQ * MHZ;
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
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    case SOC_MOD_CLK_RTC_FAST:
        clk_src_freq = 20 * MHZ;
        break;
#endif
    case SOC_MOD_CLK_SPLL:
        clk_src_freq = CLK_LL_PLL_480M_FREQ_MHZ * MHZ;
        break;
    case SOC_MOD_CLK_RC_FAST:
        // C5-beta3 unable to calibrate to get exact RC_FAST frequency
        clk_src_freq = SOC_CLK_RC_FAST_FREQ_APPROX;
        break;
    case SOC_MOD_CLK_XTAL_D2:
        clk_src_freq = (CONFIG_XTAL_FREQ * MHZ) >> 1;
        break;
    default:
        break;
    }

    ESP_RETURN_ON_FALSE(clk_src_freq, ESP_FAIL, TAG,
                        "freq shouldn't be 0, calibration failed");
    *freq_value = clk_src_freq;
    return ESP_OK;
}
