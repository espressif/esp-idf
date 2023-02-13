/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>

int main (void)
{
    /* Make sure ULP core crashes by doing a NULL pointer access */
    uint32_t *null_ptr = NULL;
    *null_ptr = 1;

    /* ulp_riscv_halt() is called automatically when main exits */
    return 0;
}
