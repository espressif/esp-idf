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
#include "driver/ledc.h"
#include "soc/rtc.h"
#include "esp_private/esp_sleep_internal.h"

static const char TAG[] = "rtc_power";

static void test_deepsleep(void)
{
    esp_sleep_enable_timer_wakeup(2000000);
    ESP_LOGI(TAG, "Entering deep sleep");
    esp_deep_sleep_start();
}

TEST_CASE("Power Test: Deepsleep (with ADC/TSEN in monitor)", "[pm]")
{
    rtc_dig_clk8m_disable();    //This is workaround for bootloader not disable 8M as digital clock source

    esp_sleep_enable_adc_tsens_monitor(true);
    test_deepsleep();
}

TEST_CASE("Power Test: Deepsleep (default)", "[pm]")
{
    rtc_dig_clk8m_disable();    //This is workaround for bootloader not disable 8M as digital clock source

    test_deepsleep();
}

TEST_CASE("Power Test: Deepsleep (ultra-low power)", "[pm]")
{
    rtc_dig_clk8m_disable();    //This is workaround for bootloader not disable 8M as digital clock source

    extern void rtc_sleep_enable_ultra_low(bool);
    rtc_sleep_enable_ultra_low(true);
    test_deepsleep();
}

static void test_lightsleep(void)
{
    esp_sleep_enable_timer_wakeup(2000000);

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

TEST_CASE("Power Test: Lightsleep (XTAL 40M)", "[pm]")
{
    rtc_dig_clk8m_disable();    //This is workaround for bootloader not disable 8M as digital clock source

    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_ON);
    test_lightsleep();
}

TEST_CASE("Power Test: Lightsleep (8M by digital)", "[pm]")
{
    rtc_dig_clk8m_disable();    //This is workaround for bootloader not disable 8M as digital clock source

    ledc_timer_config_t config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .timer_num = 0,
        .freq_hz = 2 * 1000,
        .clk_cfg = LEDC_USE_RTC8M_CLK,
    };
    ledc_timer_config(&config);
    test_lightsleep();
}

TEST_CASE("Power Test: Lightsleep (with ADC/TSEN in monitor)", "[pm]")
{
    rtc_dig_clk8m_disable();    //This is workaround for bootloader not disable 8M as digital clock source

    extern void esp_sleep_enable_adc_tsens_monitor(bool);
    esp_sleep_enable_adc_tsens_monitor(true);
    test_lightsleep();
}

TEST_CASE("Power Test: Lightsleep (default)", "[pm]")
{
    rtc_dig_clk8m_disable();    //This is workaround for bootloader not disable 8M as digital clock source
    test_lightsleep();
}
