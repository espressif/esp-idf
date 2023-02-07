/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_sleep.h"
#include "unity.h"
#include "unity_test_utils.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "soc/soc_caps.h"


static const char TAG[] = "rtc_8m";

static void test_deepsleep(bool force_rtc_periph)
{
    esp_sleep_enable_timer_wakeup(2000000);
#if SOC_PM_SUPPORT_RTC_PERIPH_PD
    if (force_rtc_periph) {
        esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    }
#else
    (void)force_rtc_periph;
#endif

    ESP_LOGI(TAG, "Entering deep sleep");
    esp_deep_sleep_start();
}

TEST_CASE("Can use 8MD256 as RTC clock source in deepsleep", "[pm]")
{
    test_deepsleep(false);
}

static void test_lightsleep(bool force_rtc_periph)
{
    esp_sleep_enable_timer_wakeup(2000000);
#if SOC_PM_SUPPORT_RTC_PERIPH_PD
    if (force_rtc_periph) {
        esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    }
#else
    (void)force_rtc_periph;
#endif

    while (true) {
        printf("Entering light sleep\n");
        /* To make sure the complete line is printed before entering sleep mode,
         * need to wait until UART TX FIFO is empty:
         */
        uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);

        /* Enter sleep mode */
        esp_light_sleep_start();

        /* Determine wake up reason */
        const char* wakeup_reason;
        switch (esp_sleep_get_wakeup_cause()) {
            case ESP_SLEEP_WAKEUP_TIMER:
                wakeup_reason = "timer";
                break;
            default:
                wakeup_reason = "other";
                break;
        }
        printf("Returned from light sleep, reason: %s\n", wakeup_reason);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

TEST_CASE("Can use 8MD256 as RTC clock source in lightsleep", "[pm]")
{
    test_lightsleep(false);
}

#if SOC_PM_SUPPORT_RTC_PERIPH_PD
TEST_CASE("Can use 8MD256 as RTC clock source in deepsleep (force rtc_periph)", "[pm]")
{
    test_deepsleep(true);
}

TEST_CASE("Can use 8MD256 as RTC clock source in lightsleep (force rtc_periph)", "[pm]")
{
    test_lightsleep(true);
}
#endif
