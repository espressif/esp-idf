/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "esp_sleep.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"

#if CONFIG_EXAMPLE_GPIO_WAKEUP
#define WAKEUP_PIN CONFIG_EXAMPLE_GPIO_WAKEUP_PIN
#define WAKEUP_MODE CONFIG_EXAMPLE_GPIO_WAKEUP_MODE

/* Index by esp_sleep_gpio_wake_up_mode_t */
static const char *const s_wakeup_mode_str[] = {
    "LOW_LEVEL",
    "HIGH_LEVEL",
#if SOC_RTC_GPIO_EDGE_WAKEUP_SUPPORTED
    "POSEDGE",
    "NEGEDGE",
    "ANYEDGE",
#endif
};

void example_deep_sleep_register_gpio_wakeup(void)
{
    const esp_sleep_gpio_wake_up_mode_t wakeup_mode = WAKEUP_MODE;
#if SOC_RTC_GPIO_EDGE_WAKEUP_SUPPORTED
    const gpio_pullup_t pull_up_en =
        ((wakeup_mode == ESP_GPIO_WAKEUP_GPIO_LOW) || (wakeup_mode == ESP_GPIO_WAKEUP_GPIO_NEGEDGE)) ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    const gpio_pulldown_t pull_down_en =
        ((wakeup_mode == ESP_GPIO_WAKEUP_GPIO_HIGH) || (wakeup_mode == ESP_GPIO_WAKEUP_GPIO_POSEDGE)) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;
#else
    const gpio_pullup_t pull_up_en = (wakeup_mode == ESP_GPIO_WAKEUP_GPIO_LOW) ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    const gpio_pulldown_t pull_down_en = (wakeup_mode == ESP_GPIO_WAKEUP_GPIO_HIGH) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;
#endif
    const gpio_config_t config = {
        .pin_bit_mask = BIT64(WAKEUP_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = pull_up_en,
        .pull_down_en = pull_down_en,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&config));
    ESP_ERROR_CHECK(esp_sleep_enable_gpio_wakeup_on_hp_periph_powerdown(BIT64(WAKEUP_PIN), wakeup_mode));

    printf("Enabling GPIO wakeup on GPIO%d, trigger=%s\n", WAKEUP_PIN, s_wakeup_mode_str[wakeup_mode]);
}
#endif
