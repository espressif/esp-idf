/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* ULP RISC-V interrupts example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   This code runs on ULP RISC-V  coprocessor
*/

#include "ulp_riscv_utils.h"
#include "ulp_riscv_gpio.h"

#define WAKE_PIN    CONFIG_EXAMPLE_GPIO_INT

int sw_int_cnt = 0;
int wake_by_sw = 0;
int wake_by_gpio = 0;

/* SW Interrupt Handler */
void sw_int_handler(void *arg)
{
    sw_int_cnt++;
}

/* GPIO Interrupt Handler */
void gpio_int_handler(void *arg)
{
    wake_by_gpio = 1;
    ulp_riscv_wakeup_main_processor();
}

int main(void) {
    /* Register SW interrupt handler */
    ulp_riscv_enable_sw_intr(sw_int_handler, NULL);

    /* Configure GPIO in input mode for interrupt */
    ulp_riscv_gpio_init(WAKE_PIN);
    ulp_riscv_gpio_input_enable(WAKE_PIN);

    /* Register GPIO interrupt handler */
    ulp_riscv_gpio_isr_register(WAKE_PIN, ULP_RISCV_GPIO_INTR_POSEDGE, gpio_int_handler, NULL);

    while (1) {
        /* Trigger SW interrupt */
        ulp_riscv_trigger_sw_intr();

        if ((sw_int_cnt % 5 == 0) && !wake_by_gpio) {
            wake_by_sw = 1;
            ulp_riscv_wakeup_main_processor();
        }

        ulp_riscv_delay_cycles(1000 * ULP_RISCV_CYCLES_PER_MS);
    }

    return 0;
}
