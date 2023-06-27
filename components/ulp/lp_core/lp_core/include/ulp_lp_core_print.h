/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief Print from the LP core
 *
 * @note This function uses the LP UART peripheral to enable prints.The LP UART must be initialized with lp_core_uart_init() before using this function.
 * @note This function is not a standard printf function and may not support all format specifiers or special characters.
 *
 * @param format        string to be printed
 * @param ...           variable argument list
 *
 */
void lp_core_printf(const char* format, ...);
