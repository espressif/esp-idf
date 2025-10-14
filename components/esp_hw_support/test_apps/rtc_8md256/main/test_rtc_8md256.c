/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "esp_sleep.h"
#include "unity.h"
#include "unity_test_utils.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "soc/soc_caps.h"


ESP_LOG_ATTR_TAG(TAG, "rtc_8m");

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

static void check_reset_reason_deep_sleep(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_DEEPSLEEP, esp_reset_reason());
}

static void enter_deepsleep_by_core1(void *args)
{
    do {
        test_deepsleep(false);
    } while (1);
}

static void request_core1_do_deepsleep(void)
{
    fflush(stdout);
    fsync(fileno(stdout));
    xTaskCreatePinnedToCore(enter_deepsleep_by_core1, "deep_sleep_task", 4096, NULL, 6, NULL, 1);
    while(1);
}

TEST_CASE_MULTIPLE_STAGES("Can use 8MD256 as RTC clock source in deepsleep (enter sleep by core1)", "[pm]",
                            request_core1_do_deepsleep,
                            check_reset_reason_deep_sleep);

static void do_cpu_reset(void)
{
    esp_restart();
}

static void check_cpu_reset_and_do_system_reset(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_SW, esp_reset_reason());
    esp_rom_software_reset_system();
}

static void check_system_reset_and_do_deepsleep(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_SW, esp_reset_reason());
    test_deepsleep(false);
}

TEST_CASE_MULTIPLE_STAGES("Can use 8MD256 as RTC clock source in deepsleep after reset", "[pm]",
                            do_cpu_reset,
                            check_cpu_reset_and_do_system_reset,
                            check_system_reset_and_do_deepsleep,
                            check_reset_reason_deep_sleep);

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

        printf("Returned from light sleep, reason: %s\n", (esp_sleep_get_wakeup_causes() & BIT(ESP_SLEEP_WAKEUP_TIMER)) ? "timer" : "other");
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
