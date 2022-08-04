/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Underlying driver function for printing a char, e.g. over UART */
typedef void (*putc_fn_t)(const void *ctx, const char c);


/**
 * @brief Installs a print driver that will be used for ulp_riscv_print calls
 *
 * @param putc Underlying driver function for printing a char, e.g. over UART
 * @param putc_ctx Context that will be passed when calling the putc function
 */
void ulp_riscv_print_install(putc_fn_t putc, void *putc_ctx);


/**
 * @brief Prints a null-terminated string
 *
 * @param str String to print
 */
void ulp_riscv_print_str(const char *str);


/**
 * @brief Prints a hex number. Does not print 0x, only the digits
 *
 * @param Hex number to print
 */
void ulp_riscv_print_hex(int h);

#ifdef __cplusplus
}
#endif
