/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"

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
#if CONFIG_ULP_ROM_PRINT_ENABLE
extern int ets_printf(const char* format, ...);
#define lp_core_printf ets_printf
#else
//TODO: Change return type from void to int in IDF 6.0
void lp_core_printf(const char* format, ...);
#endif /* CONFIG_ULP_ROM_PRINT_ENABLE */

#if CONFIG_ULP_ROM_PRINT_ENABLE
/**
 * @brief Install LP ROM UART printf function as standard putc handler to enable prints
 *
 * @note This function must be called before printing anything when the LP core boots from LP ROM but does not install
 *       putc handler. This is possible when the LP ROM is instructed so by setting bit#1 in the LP_SYSTEM_REG_LP_STORE9_REG register.
 *       Disabling ROM UART init is default behavior in IDF, since the clock configured by the ROM code for UART (XTAL) is normally
 *       powered down during sleep.
 */
extern void ets_install_uart_printf(void);
#define lp_core_install_uart_print ets_install_uart_printf
#endif /* CONFIG_ULP_ROM_PRINT_ENABLE */

/**
 * @brief Print a single character from the LP core
 *
 * @param c     character to be printed
 */
void lp_core_print_char(char c);

/**
 * @brief Print a null-terminated string from the LP core
 *
 * @param str    null-terminated string to be printed
 */
void lp_core_print_str(const char *str);

/**
 * @brief Print a hex value from the LP core
 *
 * @param h     hex value to be printed
 *
 * @note Does not print '0x', only the digits (will always print 8 digits)
 */
void lp_core_print_hex(int h);

/**
 * @brief Print a two digit integer from the LP-Core
 *
 * @param d     integer to be printed
 */
void lp_core_print_dec_two_digits(int d);

#ifdef __cplusplus
}
#endif
