/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include "esp_sleep.h"
#include "sdkconfig.h"
#include "driver/rtc_io.h"

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
    ESP_ERROR_CHECK(rtc_gpio_pullup_dis(ext_wakeup_pin_0));
    ESP_ERROR_CHECK(rtc_gpio_pulldown_en(ext_wakeup_pin_0));
}
#endif // CONFIG_EXAMPLE_EXT0_WAKEUP

#if CONFIG_EXAMPLE_EXT1_WAKEUP
void example_deep_sleep_register_ext1_wakeup(void)
{
    const int ext_wakeup_pin_1 = 2;
    const uint64_t ext_wakeup_pin_1_mask = 1ULL << ext_wakeup_pin_1;
    const int ext_wakeup_pin_2 = 4;
    const uint64_t ext_wakeup_pin_2_mask = 1ULL << ext_wakeup_pin_2;

    printf("Enabling EXT1 wakeup on pins GPIO%d, GPIO%d\n", ext_wakeup_pin_1, ext_wakeup_pin_2);
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(ext_wakeup_pin_1_mask | ext_wakeup_pin_2_mask, ESP_EXT1_WAKEUP_ANY_HIGH));

    /* If there are no external pull-up/downs, tie wakeup pins to inactive level with internal pull-up/downs via RTC IO
     * during deepsleep. However, RTC IO relies on the RTC_PERIPH power domain. Keeping this power domain on will
     * increase some power comsumption. */
#if CONFIG_EXAMPLE_EXT1_USE_INTERNAL_PULLUPS
    ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON));
    ESP_ERROR_CHECK(rtc_gpio_pullup_dis(ext_wakeup_pin_1));
    ESP_ERROR_CHECK(rtc_gpio_pulldown_en(ext_wakeup_pin_1));
    ESP_ERROR_CHECK(rtc_gpio_pullup_dis(ext_wakeup_pin_2));
    ESP_ERROR_CHECK(rtc_gpio_pulldown_en(ext_wakeup_pin_2));
#endif //CONFIG_EXAMPLE_EXT1_USE_INTERNAL_PULLUPS
}

#endif // CONFIG_EXAMPLE_EXT1_WAKEUP
