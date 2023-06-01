/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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

#include "ulp_riscv.h"
#include "ulp_riscv_utils.h"

int main (void)
{
    ulp_riscv_wakeup_main_processor();

    /* Wakeup interrupt is a level interrupt, wait 1 sec to
       allow user to release button to avoid waking up the ULP multiple times */
    ulp_riscv_delay_cycles(1000*1000 * ULP_RISCV_CYCLES_PER_US);
    ulp_riscv_gpio_wakeup_clear();

    /* ulp_riscv_halt() is called automatically when main exits */
    return 0;
}
