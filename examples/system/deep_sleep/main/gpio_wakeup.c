/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "sdkconfig.h"

#if CONFIG_EXAMPLE_GPIO_WAKEUP
#define DEFAULT_WAKEUP_PIN      CONFIG_EXAMPLE_GPIO_WAKEUP_PIN
#ifdef CONFIG_EXAMPLE_GPIO_WAKEUP_HIGH_LEVEL
#define DEFAULT_WAKEUP_LEVEL    ESP_GPIO_WAKEUP_GPIO_HIGH
#else
#define DEFAULT_WAKEUP_LEVEL    ESP_GPIO_WAKEUP_GPIO_LOW
#endif

void example_deep_sleep_register_gpio_wakeup(void)
{
    const gpio_config_t config = {
        .pin_bit_mask = BIT(DEFAULT_WAKEUP_PIN),
        .mode = GPIO_MODE_INPUT,
    };

    ESP_ERROR_CHECK(gpio_config(&config));
    ESP_ERROR_CHECK(esp_sleep_enable_gpio_wakeup_on_hp_periph_powerdown(BIT(DEFAULT_WAKEUP_PIN), DEFAULT_WAKEUP_LEVEL));

    printf("Enabling GPIO wakeup on pins GPIO%d\n", DEFAULT_WAKEUP_PIN);
}
#endif
