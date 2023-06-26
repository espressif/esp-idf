/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

volatile uint32_t riscv_counter2 = 0;

int main (void)
{
    riscv_counter2++;

    /* ulp_riscv_halt() is called automatically when main exits */
    return 0;
}
