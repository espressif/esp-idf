/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Add two integers
 *
 * @param a First operand
 * @param b Second operand
 * @return Sum of a and b
 */
int math_add(int a, int b);

/**
 * @brief Subtract two integers
 *
 * @param a First operand
 * @param b Second operand
 * @return a minus b
 */
int math_subtract(int a, int b);

/**
 * @brief Multiply two integers
 *
 * @param a First operand
 * @param b Second operand
 * @return Product of a and b
 */
int math_multiply(int a, int b);

#ifdef __cplusplus
}
#endif
