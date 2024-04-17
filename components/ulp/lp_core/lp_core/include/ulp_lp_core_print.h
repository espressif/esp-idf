/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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
int (*lp_core_printf)(const char* format, ...) = ets_printf;
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
void (*lp_core_install_uart_printf)(void) = ets_install_uart_printf;
#endif /* CONFIG_ULP_ROM_PRINT_ENABLE */
