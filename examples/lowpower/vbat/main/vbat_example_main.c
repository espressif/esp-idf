/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <time.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "esp_vbat.h"
#include "esp_pm.h"
#include "esp_rtc_time.h"
#include "esp_sleep.h"

void app_main(void)
{
#if CONFIG_EXAMPLE_WAITING_BATTERY_CHARGING_IN_LIGHT_SLEEP
    esp_pm_config_t pm_config = {
        .max_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
        .min_freq_mhz = CONFIG_XTAL_FREQ,
        .light_sleep_enable = true
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
#endif

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_VBAT_UNDER_VOLT) {
#if CONFIG_ESP_VBAT_USE_RECHARGEABLE_BATTERY
        printf("Wake up from VBAT low power\n");
#else
        printf("Wake up from VBAT brownout\n");
#endif
    } else if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
        printf("Wake up from Timer\n");
    } else {
        printf("Not a deep sleep reset\n");
    }

    esp_err_t sleep_result;
    do {
#if CONFIG_ESP_VBAT_USE_RECHARGEABLE_BATTERY
        if (esp_vbat_get_battery_state() == ESP_VBAT_STATE_CHARGING) {
#if CONFIG_EXAMPLE_WAITING_BATTERY_CHARGING_IN_DEEP_SLEEP
            printf("Battery is charging, wake up the chip every %d seconds to check whether the battery is charged done !\n", CONFIG_EXAMPLE_VBAT_CHARGING_DONE_CHECK_PERIOD);
            esp_sleep_enable_timer_wakeup(CONFIG_EXAMPLE_VBAT_CHARGING_DONE_CHECK_PERIOD * 1000 * 1000);
#else
            printf("Battery is low, waiting for charging to complete before going to deep sleep!\n");
            do {
                // Task will enter block state in `esp_vbat_wait_battery_charge_done`, staying in active state or
                // entering lightsleep is determined by esp_pm configuration.
                if (esp_vbat_wait_battery_charge_done(CONFIG_EXAMPLE_VBAT_CHARGING_DONE_CHECK_PERIOD * 1000 / portTICK_PERIOD_MS) == ESP_OK) {
                    printf("Battery charging done!\n");
                    break;
                }
            } while (1);
#endif
        }
#else
        if (esp_vbat_get_battery_state() == ESP_VBAT_STATE_LOWBATTERY) {
            printf("Battery is low, VBAT power will not be used during deep sleep!\n");
        }
#endif

#if CONFIG_EXAMPLE_WAITING_BATTERY_CHARGING_IN_LIGHT_SLEEP
        // Disable auto-lightsleep configured timer wakeup source here.
        pm_config.light_sleep_enable = false;
        ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
#endif

        // enter deep sleep
        printf("Current RTC Time: %lld (us)\n", esp_rtc_get_time_us());
        printf("Entering deep sleep\n");
        sleep_result = esp_deep_sleep_try_to_start();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("Failed to enter deepsleep, please check wakeup source setting and state!\n");
    } while (sleep_result != ESP_OK);
}
