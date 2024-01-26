/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include "ulp_riscv_utils.h"

int main(void)
{
    // Wait for the main core in the test case to enter lightsleep
    ulp_riscv_delay_cycles(100 * ULP_RISCV_CYCLES_PER_MS);
    /* Make sure ULP core crashes by doing a NULL pointer access */
    uint32_t *null_ptr = NULL;
    *null_ptr = 1;

    /* ulp_riscv_halt() is called automatically when main exits */
    return 0;
}
