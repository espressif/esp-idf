/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include "ulp_lp_core_print.h"

volatile int hex_value = 0x1234ABCD;
volatile int dec_value = 56;

int main(void)
{
    lp_core_printf("Hello, World!\n");

    lp_core_print_hex(hex_value);
    lp_core_print_char('\n');

    lp_core_print_dec_two_digits(dec_value);
    lp_core_print_char('\n');

    return 0;
}
