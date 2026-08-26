/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include "esp_sleep.h"
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "driver/gpio.h"

#if CONFIG_EXAMPLE_EXT0_WAKEUP
#if CONFIG_IDF_TARGET_ESP32
const int ext_wakeup_pin_0 = 25;
#else
const int ext_wakeup_pin_0 = 3;
#endif

void example_deep_sleep_register_ext0_wakeup(void)
{
    printf("Enabling EXT0 wakeup on pin GPIO%d\n", ext_wakeup_pin_0);
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(ext_wakeup_pin_0, 1));

    // Configure pullup/downs via RTCIO to tie wakeup pins to inactive level during deepsleep.
    // EXT0 resides in the same power domain (RTC_PERIPH) as the RTC IO pullup/downs.
    // No need to keep that power domain explicitly, unlike EXT1.
    ESP_ERROR_CHECK(rtc_gpio_init(ext_wakeup_pin_0));
    ESP_ERROR_CHECK(rtc_gpio_pullup_dis(ext_wakeup_pin_0));
    ESP_ERROR_CHECK(rtc_gpio_pulldown_en(ext_wakeup_pin_0));
}
#endif // CONFIG_EXAMPLE_EXT0_WAKEUP

#if CONFIG_EXAMPLE_EXT1_WAKEUP
void example_deep_sleep_register_ext1_wakeup(void)
{
    const int ext_wakeup_pin_1 = CONFIG_EXAMPLE_EXT1_WAKEUP_PIN_1;
    const int ext_wakeup_pin_2 = CONFIG_EXAMPLE_EXT1_WAKEUP_PIN_2;
    const uint64_t ext_wakeup_pin_1_mask = 1ULL << ext_wakeup_pin_1;
    const uint64_t ext_wakeup_pin_2_mask = 1ULL << ext_wakeup_pin_2;
    printf("Enabling EXT1 wakeup on pins GPIO%d, GPIO%d\n", ext_wakeup_pin_1, ext_wakeup_pin_2);

#if SOC_PM_SUPPORT_EXT1_WAKEUP_MODE_PER_PIN
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup_io(ext_wakeup_pin_1_mask, CONFIG_EXAMPLE_EXT1_WAKEUP_MODE_PIN_1));
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup_io(ext_wakeup_pin_2_mask, CONFIG_EXAMPLE_EXT1_WAKEUP_MODE_PIN_2));
#else
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup_io(ext_wakeup_pin_1_mask | ext_wakeup_pin_2_mask, CONFIG_EXAMPLE_EXT1_WAKEUP_MODE));
#endif

    /* If there are no external pull-up/downs, tie wakeup pins to inactive level with internal pull-up/downs via RTC IO
     * during deepsleep. However, RTC IO relies on the RTC_PERIPH power domain. Keeping this power domain on will
     * increase some power consumption. However, if we turn off the RTC_PERIPH domain or if certain chips lack the RTC_PERIPH
     * domain, we will use the HOLD feature to maintain the pull-up and pull-down on the pins during sleep.*/
#if CONFIG_EXAMPLE_EXT1_USE_INTERNAL_PULLUPS
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
        ESP_ERROR_CHECK(rtc_gpio_init(ext_wakeup_pin_1));
        ESP_ERROR_CHECK(rtc_gpio_init(ext_wakeup_pin_2));
#if SOC_PM_SUPPORT_EXT1_WAKEUP_MODE_PER_PIN
    if (CONFIG_EXAMPLE_EXT1_WAKEUP_MODE_PIN_1) {
        ESP_ERROR_CHECK(rtc_gpio_pullup_dis(ext_wakeup_pin_1));
        ESP_ERROR_CHECK(rtc_gpio_pulldown_en(ext_wakeup_pin_1));
    } else {
        ESP_ERROR_CHECK(rtc_gpio_pulldown_dis(ext_wakeup_pin_1));
        ESP_ERROR_CHECK(rtc_gpio_pullup_en(ext_wakeup_pin_1));
    }
    if (CONFIG_EXAMPLE_EXT1_WAKEUP_MODE_PIN_2) {
        ESP_ERROR_CHECK(rtc_gpio_pullup_dis(ext_wakeup_pin_2));
        ESP_ERROR_CHECK(rtc_gpio_pulldown_en(ext_wakeup_pin_2));
    } else {
        ESP_ERROR_CHECK(rtc_gpio_pulldown_dis(ext_wakeup_pin_2));
        ESP_ERROR_CHECK(rtc_gpio_pullup_en(ext_wakeup_pin_2));
    }
#else // !SOC_PM_SUPPORT_EXT1_WAKEUP_MODE_PER_PIN
    if (CONFIG_EXAMPLE_EXT1_WAKEUP_MODE) {
        ESP_ERROR_CHECK(rtc_gpio_pullup_dis(ext_wakeup_pin_1));
        ESP_ERROR_CHECK(rtc_gpio_pulldown_en(ext_wakeup_pin_1));
        ESP_ERROR_CHECK(rtc_gpio_pullup_dis(ext_wakeup_pin_2));
        ESP_ERROR_CHECK(rtc_gpio_pulldown_en(ext_wakeup_pin_2));
    } else {
        ESP_ERROR_CHECK(rtc_gpio_pulldown_dis(ext_wakeup_pin_1));
        ESP_ERROR_CHECK(rtc_gpio_pullup_en(ext_wakeup_pin_1));
        ESP_ERROR_CHECK(rtc_gpio_pulldown_dis(ext_wakeup_pin_2));
        ESP_ERROR_CHECK(rtc_gpio_pullup_en(ext_wakeup_pin_2));
    }
#endif
#else // ! SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
    const gpio_config_t config = {
        .pin_bit_mask = BIT(ext_wakeup_pin_1) | BIT(ext_wakeup_pin_2),
        .mode = GPIO_MODE_INPUT,
    };
    ESP_ERROR_CHECK(gpio_config(&config));
#if SOC_PM_SUPPORT_EXT1_WAKEUP_MODE_PER_PIN
    if (CONFIG_EXAMPLE_EXT1_WAKEUP_MODE_PIN_1) {
        gpio_pullup_dis(ext_wakeup_pin_1);
        gpio_pulldown_en(ext_wakeup_pin_1);
    } else {
        gpio_pulldown_dis(ext_wakeup_pin_1);
        gpio_pullup_en(ext_wakeup_pin_1);
    }
    if (CONFIG_EXAMPLE_EXT1_WAKEUP_MODE_PIN_2) {
        gpio_pullup_dis(ext_wakeup_pin_2);
        gpio_pulldown_en(ext_wakeup_pin_2);
    } else {
        gpio_pulldown_dis(ext_wakeup_pin_2);
        gpio_pullup_en(ext_wakeup_pin_2);
    }
#else
    if (CONFIG_EXAMPLE_EXT1_WAKEUP_MODE) {
        ESP_ERROR_CHECK(gpio_pullup_dis(ext_wakeup_pin_1));
        ESP_ERROR_CHECK(gpio_pulldown_en(ext_wakeup_pin_1));
        ESP_ERROR_CHECK(gpio_pullup_dis(ext_wakeup_pin_2));
        ESP_ERROR_CHECK(gpio_pulldown_en(ext_wakeup_pin_2));
    } else {
        ESP_ERROR_CHECK(gpio_pulldown_dis(ext_wakeup_pin_1));
        ESP_ERROR_CHECK(gpio_pullup_en(ext_wakeup_pin_1));
        ESP_ERROR_CHECK(gpio_pulldown_dis(ext_wakeup_pin_2));
        ESP_ERROR_CHECK(gpio_pullup_en(ext_wakeup_pin_2));
    }
#endif
#endif
#endif // CONFIG_EXAMPLE_EXT1_USE_INTERNAL_PULLUPS
}
#endif // CONFIG_EXAMPLE_EXT1_WAKEUP
