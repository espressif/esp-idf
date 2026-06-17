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

typedef struct {
    soc_module_clk_t clk_upstream;
    uint32_t divider;
} esp_clk_tree_src_config_t;

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
 *        This function is only used in rtc_clk.c for now.
 *
 * @param[in] clk_circuit Clock circuits, in soc_root_clk_circuit_t
 * @param[in] enable  Enable / Disable the power of the clock circuit
 *
 * @return True if power control is truly toggled, false otherwise
 */
bool esp_clk_tree_enable_power(soc_root_clk_circuit_t clk_circuit, bool enable);

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

/**
 * @brief Select the upstream PLL that feeds a derived clock
 *
 * Programs the upstream-mux of `clk_src` (e.g. `SOC_MOD_CLK_PLL_F50M`) to
 * source from `upstream` (e.g. `SOC_MOD_CLK_MPLL`, `SOC_MOD_CLK_CPLL`). The
 * clock must already be acquired via `esp_clk_tree_enable_src(clk_src, true)`.
 *
 * Use `esp_clk_tree_src_set_freq_hz()` afterwards to choose the divider for
 * the desired output frequency. If `select_upstream` is not called before
 * `set_freq_hz`, the engine auto-picks the first upstream in the descriptor
 * whose current frequency divides cleanly to the requested target.
 *
 * On targets where the derived clock has no upstream mux (e.g. ESP32-P4
 * `PLL_F50M` is fixed to MPLL), this function still validates that the
 * requested upstream matches the fixed source and updates internal state,
 * but does not touch any hardware register.
 *
 * Has effect only on "derived" clocks (those with a descriptor registered
 * via `esp_clk_tree_get_derived_clk_desc()`). For other clocks returns
 * `ESP_ERR_NOT_SUPPORTED`.
 *
 * Share-lock semantics: when more than one peripheral has acquired
 * `clk_src`, only a request that matches the upstream already programmed by
 * the first peer succeeds. Any other request returns
 * `ESP_ERR_INVALID_STATE`.
 *
 * @param[in] clk_src   Derived module clock id (e.g. `SOC_MOD_CLK_PLL_F50M`)
 * @param[in] upstream  Upstream PLL to route from (e.g. `SOC_MOD_CLK_MPLL`)
 * @return
 *      - ESP_OK                 Upstream selected
 *      - ESP_ERR_INVALID_ARG    `upstream` is not in this clock's allowed list
 *      - ESP_ERR_INVALID_STATE  Clock is shared and another peripheral has
 *                               already selected a different upstream
 *      - ESP_ERR_NOT_SUPPORTED  `clk_src` has no derived-clock descriptor on
 *                               this target
 */
esp_err_t esp_clk_tree_src_select_upstream(soc_module_clk_t clk_src,
                                           soc_module_clk_t upstream);

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
