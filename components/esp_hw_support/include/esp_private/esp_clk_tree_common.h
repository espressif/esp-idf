/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_clk_tree.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_CLK_RC_FAST_D256_SUPPORTED
/**
 * @brief Get frequency of RC_FAST_D256_CLK
 *
 * @param precision Degree of precision of the returned frequency value, one of esp_clk_tree_src_freq_precision_t values
 *
 * @return RC_FAST_D256 clock frequency, in Hz. Returns 0 if degree of precision is invalid or calibration failed.
 */
uint32_t esp_clk_tree_rc_fast_d256_get_freq_hz(esp_clk_tree_src_freq_precision_t precision);
#endif

#if SOC_CLK_XTAL32K_SUPPORTED
/**
 * @brief Get frequency of XTAL32K_CLK
 *
 * @param precision Degree of precision of the returned frequency value, one of esp_clk_tree_src_freq_precision_t values
 *
 * @return XTAL32K clock frequency, in Hz. Returns 0 if degree of precision is invalid or calibration failed.
 */
uint32_t esp_clk_tree_xtal32k_get_freq_hz(esp_clk_tree_src_freq_precision_t precision);
#endif

#if SOC_CLK_OSC_SLOW_SUPPORTED
/**
 * @brief Get frequency of OSC_SLOW_CLK
 *
 * @param precision Degree of precision of the returned frequency value, one of esp_clk_tree_src_freq_precision_t values
 *
 * @return OSC_SLOW clock frequency, in Hz. Returns 0 if degree of precision is invalid or calibration failed.
 */
uint32_t esp_clk_tree_osc_slow_get_freq_hz(esp_clk_tree_src_freq_precision_t precision);
#endif

/**
 * @brief Get frequency of RC_FAST_CLK
 *
 * @param precision Degree of precision of the returned frequency value, one of esp_clk_tree_src_freq_precision_t values
 *
 * @return RC_FAST clock frequency, in Hz. Returns 0 if degree of precision is invalid or calibration failed.
 */
uint32_t esp_clk_tree_rc_fast_get_freq_hz(esp_clk_tree_src_freq_precision_t precision);

/**
 * @brief Get frequency of LP_SLOW_CLK (i.e. RTC_SLOW_CLK)
 *
 * @param precision Degree of precision of the returned frequency value, one of esp_clk_tree_src_freq_precision_t values
 *
 * @return LP_SLOW clock frequency, in Hz. Returns 0 if degree of precision is invalid or calibration failed.
 */
uint32_t esp_clk_tree_lp_slow_get_freq_hz(esp_clk_tree_src_freq_precision_t precision);

/**
 * @brief Get frequency of LP_FAST_CLK (i.e. RTC_FAST_CLK)
 *
 * @param precision Degree of precision of the returned frequency value, one of esp_clk_tree_src_freq_precision_t values
 *
 * @return LP_FAST clock frequency, in Hz. Returns 0 if degree of precision is invalid or calibration failed.
 */
uint32_t esp_clk_tree_lp_fast_get_freq_hz(esp_clk_tree_src_freq_precision_t precision);

/**
 * @brief Enable / Disable the clock gate of the clock source
 *
 * @param[in] clk_src Clock source available to modules, in soc_module_clk_t
 * @param[in] enable  Enable / Disable the clock gate
 *
 * @note !!! WARNING !!!
 *       There's no reference counter to protect the clock source status, the caller should use the interface
 *       with CAUTION to disable the clock source to avoid damaging other peripherals that are dependent on
 *       the clock source.
 *
 * @return
 *      - ESP_OK               Success
 *      - ESP_ERR_INVALID_ARG  Parameter error
 */
esp_err_t esp_clk_tree_enable_src(soc_module_clk_t clk_src, bool enable);

#ifdef __cplusplus
}
#endif
