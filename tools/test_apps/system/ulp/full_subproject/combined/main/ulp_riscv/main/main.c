/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>
#include "ulp_riscv_utils.h"

uint32_t riscv_counter = 0;

int main(void)
{
    riscv_counter++;
    if (riscv_counter >= 10) {
        ulp_riscv_wakeup_main_processor();
    }

    return 0;
}
