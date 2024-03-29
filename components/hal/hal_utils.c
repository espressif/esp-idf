/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/hal_utils.h"
#include "hal/assert.h"

__attribute__((always_inline))
static inline uint32_t _sub_abs(uint32_t a, uint32_t b)
{
    return a > b ? a - b : b - a;
}

uint32_t hal_utils_calc_clk_div_frac_fast(const hal_utils_clk_info_t *clk_info, hal_utils_clk_div_t *clk_div)
{
    HAL_ASSERT(clk_info->max_fract > 2);
    uint32_t div_denom = 2;
    uint32_t div_numer = 0;
    uint32_t div_integ = clk_info->src_freq_hz / clk_info->exp_freq_hz;
    uint32_t freq_error = clk_info->src_freq_hz % clk_info->exp_freq_hz;

    // fractional divider
    if (freq_error) {
        // Carry bit if the decimal is greater than 1.0 - 1.0 / ((max_fract - 1) * 2)
        if (freq_error < clk_info->exp_freq_hz - clk_info->exp_freq_hz / (clk_info->max_fract - 1) * 2) {
            // Calculate the Greatest Common Divisor, time complexity O(log n)
            uint32_t gcd = hal_utils_gcd(clk_info->exp_freq_hz, freq_error);
            // divide by the Greatest Common Divisor to get the accurate fraction before normalization
            div_denom = clk_info->exp_freq_hz / gcd;
            div_numer = freq_error / gcd;
            // normalize div_denom and div_numer
            uint32_t d = div_denom / clk_info->max_fract + 1;
            // divide by the normalization coefficient to get the denominator and numerator within range of clk_info->max_fract
            div_denom /= d;
            div_numer /= d;
        } else {
            div_integ++;
        }
    }

    // If the expect frequency is too high or too low to satisfy the integral division range, failed and return 0
    if (div_integ < clk_info->min_integ || div_integ >= clk_info->max_integ || div_integ == 0) {
        return 0;
    }

    // Assign result
    clk_div->integer     = div_integ;
    clk_div->denominator = div_denom;
    clk_div->numerator   = div_numer;

    // Return the actual frequency
    if (div_numer) {
        uint32_t temp = div_integ * div_denom + div_numer;
        return (uint32_t)(((uint64_t)clk_info->src_freq_hz * div_denom + temp / 2) / temp);
    }
    return clk_info->src_freq_hz / div_integ;
}

uint32_t hal_utils_calc_clk_div_frac_accurate(const hal_utils_clk_info_t *clk_info, hal_utils_clk_div_t *clk_div)
{
    HAL_ASSERT(clk_info->max_fract > 2);
    uint32_t div_denom = 2;
    uint32_t div_numer = 0;
    uint32_t div_integ = clk_info->src_freq_hz / clk_info->exp_freq_hz;
    uint32_t freq_error = clk_info->src_freq_hz % clk_info->exp_freq_hz;

    if (freq_error) {
        // Carry bit if the decimal is greater than 1.0 - 1.0 / ((max_fract - 1) * 2)
        if (freq_error < clk_info->exp_freq_hz - clk_info->exp_freq_hz / (clk_info->max_fract - 1) * 2) {
            // Search the closest fraction, time complexity O(n)
            for (uint32_t sub = 0, a = 2, b = 0, min = UINT32_MAX; min && a < clk_info->max_fract; a++) {
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

    // If the expect frequency is too high or too low to satisfy the integral division range, failed and return 0
    if (div_integ < clk_info->min_integ || div_integ >= clk_info->max_integ || div_integ == 0) {
        return 0;
    }

    // Assign result
    clk_div->integer     = div_integ;
    clk_div->denominator = div_denom;
    clk_div->numerator   = div_numer;

    // Return the actual frequency
    if (div_numer) {
        uint32_t temp = div_integ * div_denom + div_numer;
        return (uint32_t)(((uint64_t)clk_info->src_freq_hz * div_denom + temp / 2) / temp);
    }
    return clk_info->src_freq_hz / div_integ;
}

uint32_t hal_utils_calc_clk_div_integer(const hal_utils_clk_info_t *clk_info, uint32_t *int_div)
{
    uint32_t div_integ = clk_info->src_freq_hz / clk_info->exp_freq_hz;
    uint32_t freq_error = clk_info->src_freq_hz % clk_info->exp_freq_hz;

    /* If there is error and always round up,
       Or, do the normal rounding and error >= (src/n + src/(n+1)) / 2,
       then carry the bit */
    if ((freq_error && clk_info->round_opt == HAL_DIV_ROUND_UP) || (clk_info->round_opt == HAL_DIV_ROUND &&
        (freq_error >= clk_info->src_freq_hz / (2 * div_integ * (div_integ + 1))))) {
        div_integ++;
    }
    /* Check the integral division whether in range [min_integ, max_integ)  */
    /* If the result is less than the minimum, set the division to the minimum but return 0 */
    if (div_integ < clk_info->min_integ) {
        *int_div = clk_info->min_integ;
        return 0;
    }
    /* if the result is greater or equal to the maximum , set the division to the maximum but return 0 */
    if (div_integ >= clk_info->max_integ) {
        *int_div = clk_info->max_integ - 1;
        return 0;
    }

    // Assign result
    *int_div = div_integ;
    // Return the actual frequency
    return clk_info->src_freq_hz / div_integ;
}
