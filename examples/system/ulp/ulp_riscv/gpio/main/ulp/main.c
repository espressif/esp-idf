/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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
#include <stdbool.h>
#include "ulp_riscv.h"
#include "ulp_riscv_utils.h"
#include "ulp_riscv_gpio.h"

static bool gpio_level = false;

/* this variable will be exported as a public symbol, visible from main CPU: */
bool gpio_level_previous = false;

int main (void)
{
    gpio_level = (bool)ulp_riscv_gpio_get_level(GPIO_NUM_0);
    gpio_level_previous = gpio_level;

    while(1) {
        gpio_level = (bool)ulp_riscv_gpio_get_level(GPIO_NUM_0);

        /* Wakes up the main CPU if pin changed its state */
        if(gpio_level != gpio_level_previous) {
            gpio_level_previous = gpio_level;
            ulp_riscv_wakeup_main_processor();
            break;
        }
    }
    /* ulp_riscv_halt() is called automatically when main exits */
    return 0;
}
