/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Integer division operation
 *
 */
typedef enum {
    HAL_DIV_ROUND_DOWN,     /*!< Round the division down to the floor integer */
    HAL_DIV_ROUND_UP,       /*!< Round the division up to the ceiling integer */
    HAL_DIV_ROUND,          /*!< Round the division to the nearest integer (round up if fraction >= 1/2, round down if fraction < 1/2) */
} hal_utils_div_round_opt_t;

/**
 * @brief Clock information
 *
 */
typedef struct {
    uint32_t src_freq_hz;   /*!< Source clock frequency, unit: Hz */
    uint32_t exp_freq_hz;   /*!< Expected output clock frequency, unit: Hz */
    uint32_t max_integ;     /*!< The max value of the integral part */
    uint32_t min_integ;     /*!< The min value of the integral part, integer range: [min_integ, max_integ) */
    union {
        uint32_t max_fract;     /*!< The max value of the denominator and numerator, numerator range: [0, max_fract), denominator range: [1, max_fract)
                                 *   Please make sure max_fract > 2 when calculate the division with fractal part */
        hal_utils_div_round_opt_t round_opt;     /*!< Integer division operation. For the case that doesn't have fractal part, set this field to the to specify the rounding method  */
    };
} hal_utils_clk_info_t;

/**
 * @brief Members of clock division
 *
 */
typedef struct {
    uint32_t integer;       /*!< Integer part of division */
    uint32_t denominator;   /*!< Denominator part of division */
    uint32_t numerator;     /*!< Numerator part of division */
} hal_utils_clk_div_t;

/**
 * @brief Calculate the clock division with fractal part fast
 * @note  Speed first algorithm, Time complexity O(log n).
 *        About 8~10 times faster than the accurate algorithm
 *
 * @param[in]  clk_info     The clock information
 * @param[out] clk_div      The clock division with integral and fractal part
 * @return
 *      - 0: Failed to get the result because the division is out of range
 *      - others: The real output clock frequency
 */
uint32_t hal_utils_calc_clk_div_frac_fast(const hal_utils_clk_info_t *clk_info, hal_utils_clk_div_t *clk_div);

/**
 * @brief Calculate the clock division with fractal part accurately
 * @note  Accuracy first algorithm, Time complexity O(n).
 *        About 1~hundreds times more accurate than the fast algorithm
 *
 * @param[in]  clk_info     The clock information
 * @param[out] clk_div      The clock division with integral and fractal part
 * @return
 *      - 0: Failed to get the result because the division is out of range
 *      - others: The real output clock frequency
 */
uint32_t hal_utils_calc_clk_div_frac_accurate(const hal_utils_clk_info_t *clk_info, hal_utils_clk_div_t *clk_div);

/**
 * @brief Calculate the clock division without fractal part
 *
 * @param[in]  clk_info     The clock information
 * @param[out] int_div      The clock integral division
 * @return
 *      - 0: Failed to get the result because the division is out of range,
 *           but parameter `int_div` will still be assigned to min/max division that given in `clk_info`,
 *           in case the caller still want to use the min/max division in this case.
 *      - others: The real output clock frequency
 */
uint32_t hal_utils_calc_clk_div_integer(const hal_utils_clk_info_t *clk_info, uint32_t *int_div);

/**
 * @brief Reverse the bit order of an 8-bit unsigned integer
 *
 * @param n The 8-bit unsigned integer to be reversed
 * @return The 8-bit unsigned integer after reversing
 */
__attribute__((always_inline))
static inline uint8_t hal_utils_bitwise_reverse8(uint8_t n)
{
    n = ((n & 0xf0) >> 4) | ((n & 0x0f) << 4);
    n = ((n & 0xcc) >> 2) | ((n & 0x33) << 2);
    n = ((n & 0xaa) >> 1) | ((n & 0x55) << 1);
    return n;
}

/**
 * @brief Helper function to calculate the GCD between two numbers using the Euclidean algorithm.
 * Calculate the Greatest Common Divisor (GDC) of two unsigned numbers
 *
 * @param num_1 First number
 * @param num_2 Second number
 * @return GCD of 'a' and 'b'
 */
__attribute__((always_inline))
static inline uint32_t hal_utils_gcd(uint32_t num_1, uint32_t num_2)
{
    uint32_t a, b, rem;
    // Always mod larger number by smaller number
    if (num_1 > num_2) {
        a = num_1;
        b = num_2;
    } else {
        b = num_2;
        a = num_1;
    }

    rem = a % b;
    while (rem != 0) {
        a = b;
        b = rem;
        rem = a % b;
    }
    return b;
}

/**
 * @brief Get the least common multiple of two integer
 *
 * @param[in]  Integer A
 * @param[in]  Integer B
 *
 * @return     LCM of A and B
 */
__attribute__((always_inline))
static inline uint32_t hal_utils_calc_lcm(uint32_t a, uint32_t b)
{
    a = a == 0 ? 1 : a;
    b = b == 0 ? 1 : b;
    return (a * b / hal_utils_gcd(a, b));
}

/**
 * @brief Fixed-point data configuration
 *
 */
typedef struct {
    uint32_t int_bit;       /*!< Integer bit of the fixed point */
    uint32_t frac_bit;      /*!< Fractional bit of the fixed point */
    bool saturation;        /*!< Whether to limit the value to the maximum when fixed-point data overflow.
                             *   When set true, the value will be limited to the maximum when the float type data is out of range.
                             *   When set false, the function will return false when the float type data is out of range.
                             */
} hal_utils_fixed_point_t;

/**
 * @brief Convert the float type to fixed point type
 * @note  The supported data format:
 *        - [input] float (IEEE 754):
 *          sign(1bit) + exponent(8bit) + mantissa(23bit)       (32 bit in total)
 *        - [output] fixed-point:
 *          sign(1bit) + integer(int_bit) + fraction(frac_bit)  (less or equal to 32 bit)
 *
 * @param[in]  flt          IEEE 754 float type data
 * @param[in]  fp_cfg       Fixed-point data configuration
 * @param[out] fp_out       The output fixed-point data
 * @return
 *      0:              Success
 *      -1:             Fixed point data overflow, `fp_out` will still be assigned
 *      -2:             Float is NaN
 *      -3:             Invalid configuration
 */
int hal_utils_float_to_fixed_point_32b(float flt, const hal_utils_fixed_point_t *fp_cfg, uint32_t *fp_out);

#ifdef __cplusplus
}
#endif
