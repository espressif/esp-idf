/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include "driver/gpio.h"
#include "ana_cmpr_example_main.h"

void example_init_monitor_gpio(void)
{
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << EXAMPLE_MONITOR_GPIO_NUM),
        .pull_down_en = false,
        .pull_up_en = false,
    };
    gpio_config(&io_conf);
    gpio_set_level(EXAMPLE_MONITOR_GPIO_NUM, 0);
}

void app_main(void)
{
#if CONFIG_EXAMPLE_USE_ETM
    example_analog_comparator_etm_app();
#else
    example_analog_comparator_intr_app();
#endif
}
