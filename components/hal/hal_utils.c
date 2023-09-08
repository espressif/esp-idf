/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/hal_utils.h"

/**
 * @brief helper function, calculate the Greatest Common Divisor
 * @note gcd(a, b) = gcd(b, a % b)
 * @param a bigger value
 * @param b smaller value
 * @return result of gcd(a, b)
 */
__attribute__((always_inline))
static inline uint32_t _gcd(uint32_t a, uint32_t b)
{
    uint32_t c = a % b;
    while (c != 0) {
        a = b;
        b = c;
        c = a % b;
    }
    return b;
}

__attribute__((always_inline))
static inline uint32_t _sub_abs(uint32_t a, uint32_t b)
{
    return a > b ? a - b : b - a;
}

uint32_t hal_utils_calc_clk_div_fast(const hal_utils_clk_info_t *clk_info, hal_utils_clk_div_t *clk_div)
{
    uint32_t div_denom = 1;
    uint32_t div_numer = 0;
    uint32_t div_integ = clk_info->src_freq_hz / clk_info->exp_freq_hz;
    uint32_t freq_error = clk_info->src_freq_hz % clk_info->exp_freq_hz;

    // If the expect frequency is too high or too low to satisfy the integral division range, failed and return 0
    if (div_integ < clk_info->min_integ || div_integ > clk_info->max_integ) {
        return 0;
    }

    // fractional divider
    if (freq_error) {
        // Calculate the Greatest Common Divisor, time complexity O(log n)
        uint32_t gcd = _gcd(clk_info->exp_freq_hz, freq_error);
        // divide by the Greatest Common Divisor to get the accurate fraction before normalization
        div_denom = clk_info->exp_freq_hz / gcd;
        div_numer = freq_error / gcd;
        // normalize div_denom and div_numer
        uint32_t d = div_denom / clk_info->max_fract + 1;
        // divide by the normalization coefficient to get the denominator and numerator within range of clk_info->max_fract
        div_denom /= d;
        div_numer /= d;
    }

    // Assign result
    clk_div->integ = div_integ;
    clk_div->denom = div_denom;
    clk_div->numer = div_numer;

    // Return the actual frequency
    if (div_numer) {
        uint32_t temp = div_integ * div_denom + div_numer;
        return (uint32_t)(((uint64_t)clk_info->src_freq_hz * div_denom + temp / 2) / temp);
    }
    return clk_info->src_freq_hz / div_integ;
}

uint32_t hal_utils_calc_clk_div_accurate(const hal_utils_clk_info_t *clk_info, hal_utils_clk_div_t *clk_div)
{
    uint32_t div_denom = 1;
    uint32_t div_numer = 0;
    uint32_t div_integ = clk_info->src_freq_hz / clk_info->exp_freq_hz;
    uint32_t freq_error = clk_info->src_freq_hz % clk_info->exp_freq_hz;

    // If the expect frequency is too high to satisfy the minimum integral division, failed and return 0
    if (div_integ < clk_info->min_integ) {
        return 0;
    }

    if (freq_error) {
        // Carry bit if the decimal is greater than 1.0 - 1.0 / (PARLIO_LL_CLK_DIVIDER_MAX * 2)
        if (freq_error < clk_info->exp_freq_hz - clk_info->exp_freq_hz / (clk_info->max_fract * 2)) {
            // Search the closest fraction, time complexity O(n)
            for (uint32_t sub = 0, a = 2, b = 0, min = UINT32_MAX; min && a <= clk_info->max_fract; a++) {
                b = (a * freq_error + clk_info->exp_freq_hz / 2) / clk_info->exp_freq_hz;
                sub = _sub_abs(clk_info->exp_freq_hz * b, freq_error * a);
                if (sub < min) {
                    div_denom = a;
                    div_numer = b;
                    min = sub;
                }
            }
        } else {
            div_integ++;
        }
    }
    // If the expect frequency is too low to satisfy the maximum integral division, failed and return 0
    if (div_integ > clk_info->max_integ) {
        return 0;
    }

    // Assign result
    clk_div->integ = div_integ;
    clk_div->denom = div_denom;
    clk_div->numer = div_numer;

    // Return the actual frequency
    if (div_numer) {
        uint32_t temp = div_integ * div_denom + div_numer;
        return (uint32_t)(((uint64_t)clk_info->src_freq_hz * div_denom + temp / 2) / temp);
    }
    return clk_info->src_freq_hz / div_integ;
}
