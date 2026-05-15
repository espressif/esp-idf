/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
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
 * @note  The clock enable status is maintained by reference counter and
 *        its status is not reset after software restart.
 *
 * @param[in] clk_src Clock source available to modules, in soc_module_clk_t
 * @param[in] enable  Enable / Disable the clock gate
 *
 * @return
 *      - ESP_OK               Success
 *      - ESP_ERR_INVALID_ARG  Parameter error
 */
esp_err_t esp_clk_tree_enable_src(soc_module_clk_t clk_src, bool enable);

/**
 * @brief Initialize clock circuit power and clock gating
 *
 * Set the clock source not in uses on the clock tree to the gated state,
 * and initialize reference counters for clock circuit power and clock gating.
 */
void esp_clk_tree_initialize(void);

/**
 * @brief Enable / Disable the power of the clock circuit
 *
 * @param[in] clk_circuit Clock circuits, in soc_root_clk_circuit_t
 * @param[in] enable  Enable / Disable the power of the clock circuit
 *
 * @return
 *      - ESP_OK               Success
 */
esp_err_t esp_clk_tree_enable_power(soc_root_clk_circuit_t clk_circuit, bool enable);

/**
 * @brief Get the power status of the clock circuit
 *
 * @param[in] clk_circuit Clock circuits, in soc_root_clk_circuit_t
 *
 * @return True if the clock circuit power is on, false otherwise
 */
bool esp_clk_tree_is_power_on(soc_root_clk_circuit_t clk_circuit);

#if SOC_CLK_APLL_SUPPORTED
/**
 * @brief Enable APLL power if it has not enabled
 *
 * @note Do not use this function in applications or drivers, please use `esp_clk_tree_enable_src` instead.
 */
void esp_clk_tree_apll_acquire(void);

/**
 * @brief Shut down APLL power if no peripherals using APLL
 *
 * @note Do not use this function in applications or drivers, please use `esp_clk_tree_enable_src` instead.
 */
void esp_clk_tree_apll_release(void);

/**
 * @brief Calculate and set APLL coefficients by given frequency
 * @note  This calculation is based on the inequality:
 *        xtal_freq * (4 + sdm2 + sdm1/256 + sdm0/65536) >= CLK_LL_APLL_MULTIPLIER_MIN_HZ(350 MHz)
 *        It will always calculate the minimum coefficients that can satisfy the inequality above, instead of loop them one by one.
 *        which means more appropriate coefficients are likely to exist.
 *        But this algorithm can meet almost all the cases and the accuracy can be guaranteed as well.
 * @note  The APLL frequency is only allowed to set when there is only one peripheral refer to it.
 *        If APLL is already set by another peripheral, this function will return `ESP_ERR_INVALID_STATE`
 *        and output the current frequency by parameter `real_freq`.
 *
 * @param expt_freq Expected APLL frequency (unit: Hz)
 * @param real_freq APLL real working frequency [output] (unit: Hz)
 * @return
 *      - ESP_OK: APLL frequency set success
 *      - ESP_ERR_INVALID_ARG: The input expt_freq is out of APLL support range
 *      - ESP_ERR_INVALID_STATE: APLL is referred by more than one peripherals, not allowed to change its frequency now
 */
esp_err_t esp_clk_tree_apll_freq_set(uint32_t expt_freq_hz, uint32_t *real_freq_hz);
#endif // SOC_CLK_APLL_SUPPORTED

#if SOC_CLK_MPLL_SUPPORTED
/**
 * @brief Enable MPLL power if it has not enabled
 *
 * @note Do not use this function in applications or drivers, please use `esp_clk_tree_enable_src` instead.
 */
esp_err_t esp_clk_tree_mpll_acquire(void);

/**
 * @brief Shut down MPLL power if no peripherals using MPLL
 *
 * @note Do not use this function in applications or drivers, please use `esp_clk_tree_enable_src` instead.
 */
void esp_clk_tree_mpll_release(void);

/**
 * @brief Configure MPLL frequency
 * @note  The MPLL frequency is only allowed to set when there is only one peripheral refer to it.
 *        If MPLL is already set by another peripheral, this function will return `ESP_ERR_INVALID_STATE`
 *        and output the current frequency by parameter `real_freq`.
 *
 * @param expt_freq Expected MPLL frequency (unit: Hz)
 * @param real_freq MPLL current working frequency [output] (unit: Hz)
 * @return
 *      - ESP_OK: MPLL frequency set success
 *      - ESP_ERR_INVALID_STATE: MPLL is referred by more than one peripherals, not allowed to change its frequency now
 */
esp_err_t esp_clk_tree_mpll_freq_set(uint32_t expt_freq_hz, uint32_t *real_freq_hz);
#endif // SOC_CLK_MPLL_SUPPORTED

#ifdef __cplusplus
}
#endif
