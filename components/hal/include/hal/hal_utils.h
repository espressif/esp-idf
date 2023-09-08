/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Clock infomation
 *
 */
typedef struct {
    uint32_t src_freq_hz;   /*!< Source clock frequency, unit: Hz */
    uint32_t exp_freq_hz;   /*!< Expected output clock frequency, unit: Hz */
    uint32_t max_integ;     /*!< The max value of the integral part */
    uint32_t min_integ;     /*!< The min value of the integral part, integer range: [min_integ, max_integ) */
    uint32_t max_fract;     /*!< The max value of the denominator and numerator, numerator range: [0, max_fract), denominator range: [1, max_fract) */
} hal_utils_clk_info_t;

/**
 * @brief Members of clock division
 *
 */
typedef struct {
    uint32_t integ;     /*!< Integer part of division */
    uint32_t denom;     /*!< Denominator part of division */
    uint32_t numer;     /*!< Numerator part of division */
} hal_utils_clk_div_t;

/**
 * @brief Calculate the clock division
 * @note  Speed first algorithm, Time complexity O(log n).
 *        About 8~10 times faster than the accurate algorithm
 *
 * @param[in]  clk_info     The clock infomation
 * @param[out] clk_div      The clock division
 * @return
 *      - 0: Failed to get the result because the division is out of range
 *      - others: The real output clock frequency
 */
uint32_t hal_utils_calc_clk_div_fast(const hal_utils_clk_info_t *clk_info, hal_utils_clk_div_t *clk_div);

/**
 * @brief Calculate the clock division
 * @note  Accuracy first algorithm, Time complexity O(n).
 *        About 1~hundreds times more accurate than the fast algorithm
 *
 * @param[in]  clk_info     The clock infomation
 * @param[out] clk_div      The clock division
 * @return
 *      - 0: Failed to get the result because the division is out of range
 *      - others: The real output clock frequency
 */
uint32_t hal_utils_calc_clk_div_accurate(const hal_utils_clk_info_t *clk_info, hal_utils_clk_div_t *clk_div);

#ifdef __cplusplus
}
#endif
