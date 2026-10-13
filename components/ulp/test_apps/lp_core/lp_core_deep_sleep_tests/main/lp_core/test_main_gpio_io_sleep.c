/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_gpio.h"
#include "lp_sleep_payload.h"
#include "lp_sleep_finish.h"

#ifndef CONFIG_TEST_LP_GPIO_SLEEP_PIN
#define CONFIG_TEST_LP_GPIO_SLEEP_PIN 2
#endif

#define GPIO_POLL_US 100

volatile uint32_t gpio_io_saw_low;
volatile uint32_t gpio_io_saw_high;
volatile uint32_t gpio_io_pulses;

static bool wait_for_level(lp_io_num_t pin, uint32_t level)
{
    for (uint32_t waited = 0; waited < LP_SLEEP_GPIO_WAIT_US; waited += GPIO_POLL_US) {
        if (ulp_lp_core_gpio_get_level(pin) == level) {
            return true;
        }
        ulp_lp_core_delay_us(GPIO_POLL_US);
    }
    return false;
}

int main(void)
{
    const lp_io_num_t pin = (lp_io_num_t)CONFIG_TEST_LP_GPIO_SLEEP_PIN;

    /* The second cycle does not reload the binary, so clear the last results. */
    gpio_io_saw_low = 0;
    gpio_io_saw_high = 0;
    gpio_io_pulses = 0;

    ulp_lp_core_delay_us(LP_SLEEP_SETTLE_US);
    ulp_lp_core_gpio_init(pin);
    ulp_lp_core_gpio_input_enable(pin);
    gpio_io_saw_low = wait_for_level(pin, 0);
    gpio_io_saw_high = gpio_io_saw_low && wait_for_level(pin, 1);

    if (gpio_io_saw_high) {
        ulp_lp_core_gpio_set_level(pin, 1);
        ulp_lp_core_gpio_output_enable(pin);
        for (int i = 0; i < LP_SLEEP_GPIO_PULSES; i++) {
            ulp_lp_core_gpio_set_level(pin, 0);
            ulp_lp_core_delay_us(LP_SLEEP_GPIO_PULSE_US);
            ulp_lp_core_gpio_set_level(pin, 1);
            ulp_lp_core_delay_us(LP_SLEEP_GPIO_PULSE_US);
            gpio_io_pulses++;
        }
        ulp_lp_core_gpio_output_disable(pin);
    }
    lp_sleep_finish();
}
