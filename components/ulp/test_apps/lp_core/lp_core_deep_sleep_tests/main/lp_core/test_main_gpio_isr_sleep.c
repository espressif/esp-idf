/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_gpio.h"
#include "ulp_lp_core_interrupts.h"
#include "lp_sleep_payload.h"
#include "lp_sleep_finish.h"

#ifndef CONFIG_TEST_LP_GPIO_SLEEP_PIN
#define CONFIG_TEST_LP_GPIO_SLEEP_PIN 2
#endif

volatile uint32_t gpio_isr_counter;

void LP_CORE_ISR_ATTR ulp_lp_core_lp_io_intr_handler(void)
{
    ulp_lp_core_gpio_clear_intr_status();
    gpio_isr_counter++;
}

int main(void)
{
    const lp_io_num_t pin = (lp_io_num_t)CONFIG_TEST_LP_GPIO_SLEEP_PIN;

    ulp_lp_core_delay_us(LP_SLEEP_SETTLE_US);
    ulp_lp_core_gpio_init(pin);
    ulp_lp_core_gpio_input_enable(pin);
    ulp_lp_core_gpio_intr_enable(pin, GPIO_INTR_NEGEDGE);
    ulp_lp_core_intr_enable();

    for (int i = 0; i < 3000 && gpio_isr_counter == 0; i++) {
        ulp_lp_core_delay_us(1000);
    }

    lp_sleep_finish();
}
