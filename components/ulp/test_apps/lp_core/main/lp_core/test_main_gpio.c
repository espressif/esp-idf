/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_gpio.h"

volatile uint32_t gpio_test_finished;
volatile uint32_t gpio_test_succeeded;


int main (void)
{
    ulp_lp_core_gpio_init(LP_IO_NUM_0);

    ulp_lp_core_gpio_input_enable(LP_IO_NUM_0);
    ulp_lp_core_gpio_output_enable(LP_IO_NUM_0);

    ulp_lp_core_gpio_set_level(LP_IO_NUM_0, 0);
    gpio_test_succeeded = (ulp_lp_core_gpio_get_level(LP_IO_NUM_0) == 0);

    ulp_lp_core_gpio_set_level(LP_IO_NUM_0, 1);
    gpio_test_succeeded &= (ulp_lp_core_gpio_get_level(LP_IO_NUM_0) == 1);

    ulp_lp_core_gpio_set_level(LP_IO_NUM_0, 0);
    gpio_test_succeeded &= (ulp_lp_core_gpio_get_level(LP_IO_NUM_0) == 0);

    gpio_test_finished = 1;

    return 0;
}
