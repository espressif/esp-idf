/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* ULP-RISC-V example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   This code runs on ULP-RISC-V  coprocessor
*/

#include <stdio.h>
#include <stdint.h>
#include "ulp_riscv_utils.h"

uint32_t iteration_count = 0;
uint32_t wakeup_iteration = 0;
__attribute__((used, section(".ulp_riscv_extra_data"))) uint32_t ldgen_section_marker = 0x12345678;

int main(void)
{
    iteration_count++;
    if (iteration_count >= wakeup_iteration) {
        ulp_riscv_wakeup_main_processor();
    }

    /* ulp_riscv_halt() is called automatically when main exits */
    return 0;
}
