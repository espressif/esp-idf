/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include "ulp_lp_core_print.h"

<<<<<<< HEAD
=======
volatile int hex_value = 0x1234ABCD;
volatile int dec_value = 56;

>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
int main(void)
{
    lp_core_printf("Hello, World!\n");

<<<<<<< HEAD
=======
    lp_core_print_hex(hex_value);
    lp_core_print_char('\n');

    lp_core_print_dec_two_digits(dec_value);
    lp_core_print_char('\n');

>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
    return 0;
}
