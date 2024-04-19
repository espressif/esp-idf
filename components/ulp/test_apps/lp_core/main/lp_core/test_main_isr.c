/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_interrupts.h"
#include "ulp_lp_core_gpio.h"
#include "hal/pmu_ll.h"

volatile uint32_t io_isr_counter = 0;
volatile uint32_t pmu_isr_counter = 0;
volatile bool isr_test_started;

void LP_CORE_ISR_ATTR ulp_lp_core_lp_io_intr_handler(void)
{
    ulp_lp_core_gpio_clear_intr_status();
    io_isr_counter++;
}

void LP_CORE_ISR_ATTR ulp_lp_core_lp_pmu_intr_handler(void)
{
    ulp_lp_core_sw_intr_clear();
    pmu_isr_counter++;
}

int main(void)
{
    ulp_lp_core_sw_intr_enable(true);

    ulp_lp_core_intr_enable();

    isr_test_started = true;

    while (1) {
        // Busy wait for the interrupts to occur
    }

    return 0;
}
