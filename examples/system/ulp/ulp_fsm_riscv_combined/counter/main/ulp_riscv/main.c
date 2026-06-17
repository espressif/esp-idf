/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/* ULP RISC-V program to increment counter */

#include <stdint.h>
#include "ulp_riscv.h"
#include "ulp_riscv_utils.h"

uint32_t riscv_counter = 0;

int main(void)
{
    /* Check if counting is complete */
    if (riscv_counter >= 500) {
        /* Wake up the main processor and halt */
        ulp_riscv_wakeup_main_processor();
        ulp_riscv_halt();
        return 0;
    }

    /* Increment counter once per wakeup */
    riscv_counter++;

    /* Continue - will be woken up again by timer */
    return 0;
}
