/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Degree of precision of frequency value to be returned by esp_clk_tree_src_get_freq_hz()
 */
typedef enum {
    ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED,   /*< Get value from the data cached by the driver; If the data is 0, then a calibration will be performed */
    ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX,   /*< Get its approxiamte frequency value */
    ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT,    /*< Always perform a calibration */
    ESP_CLK_TREE_SRC_FREQ_PRECISION_INVALID,  /*< Invalid degree of precision */
} esp_clk_tree_src_freq_precision_t;

/**
 * @brief Get frequency of module clock source
 *
 * @param[in] clk_src Clock source available to modules, in soc_module_clk_t
 * @param[in] precision Degree of precision, one of esp_clk_tree_src_freq_precision_t values
 *                      This arg only applies to the clock sources that their frequencies can vary:
 *                      SOC_MOD_CLK_RTC_FAST, SOC_MOD_CLK_RTC_SLOW, SOC_MOD_CLK_RC_FAST, SOC_MOD_CLK_RC_FAST_D256,
 *                      SOC_MOD_CLK_XTAL32K
 *                      For other clock sources, this field is ignored.
 * @param[out] freq_value Frequency of the clock source, in Hz
 *
 * @return
 *      - ESP_OK               Success
 *      - ESP_ERR_INVALID_ARG  Parameter error
 *      - ESP_FAIL             Calibration failed
 */
esp_err_t esp_clk_tree_src_get_freq_hz(soc_module_clk_t clk_src, esp_clk_tree_src_freq_precision_t precision,
uint32_t *freq_value);

#ifdef __cplusplus
}
#endif
