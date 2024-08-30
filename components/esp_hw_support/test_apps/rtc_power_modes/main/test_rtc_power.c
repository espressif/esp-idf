/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
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
#include "sdkconfig.h"

static const char TAG[] = "rtc_power";

static void check_deepsleep_reset(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_DEEPSLEEP, esp_reset_reason());
}

static void test_deepsleep(void)
{
    esp_sleep_enable_timer_wakeup(2000000);
    ESP_LOGI(TAG, "Entering deep sleep");
    esp_deep_sleep_start();
}


static void test_set_adc_tsen_monitor_mode(void) {
    esp_sleep_sub_mode_config(ESP_SLEEP_USE_ADC_TSEN_MONITOR_MODE, true);
}

static void test_unset_adc_tesen_monitor_mode(void) {
    esp_sleep_sub_mode_config(ESP_SLEEP_USE_ADC_TSEN_MONITOR_MODE, false);
    TEST_ASSERT_EQUAL(0, esp_sleep_sub_mode_dump_config(NULL)[ESP_SLEEP_USE_ADC_TSEN_MONITOR_MODE]);
}

// Deepsleep (with 8MD256 or ADC/TSEN in monitor)
TEST_CASE_MULTIPLE_STAGES(  "Power Test: DSLP_8MD256", "[pm]",
                            test_set_adc_tsen_monitor_mode,
                            test_deepsleep,
                            check_deepsleep_reset,
                            test_unset_adc_tesen_monitor_mode
                        )

#if !CONFIG_RTC_CLK_SRC_INT_8MD256
// Deepsleep (default)
TEST_CASE_MULTIPLE_STAGES(  "Power Test: DSLP_DEFAULT", "[pm]",
                            test_deepsleep,
                            check_deepsleep_reset
                        )

static void test_set_ultra_low_mode(void) {
    esp_sleep_sub_mode_config(ESP_SLEEP_ULTRA_LOW_MODE, true);
}

static void test_unset_ultra_low_mode(void) {
    esp_sleep_sub_mode_config(ESP_SLEEP_ULTRA_LOW_MODE, false);
    TEST_ASSERT_EQUAL(0, esp_sleep_sub_mode_dump_config(NULL)[ESP_SLEEP_ULTRA_LOW_MODE]);
}

// Deepsleep (ultra-low power)
TEST_CASE_MULTIPLE_STAGES(  "Power Test: DSLP_ULTRA_LOW", "[pm]",
                            test_set_ultra_low_mode,
                            test_deepsleep,
                            check_deepsleep_reset,
                            test_unset_ultra_low_mode
    )

#endif //!CONFIG_RTC_CLK_SRC_INT_8MD256

static void test_lightsleep(void)
{
    esp_sleep_enable_timer_wakeup(2000000);
    int count = 5;

    while (count--) {
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

// Lightsleep (XTAL 40M)
TEST_CASE("Power Test: LSLP_XTAL_FPU", "[pm]")
{
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_ON);

    test_lightsleep();
}

// Lightsleep (8M by digital)
TEST_CASE("Power Test: LSLP_LEDC8M", "[pm]")
{
    ledc_timer_config_t config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .timer_num = 0,
        .freq_hz = 200,
        .clk_cfg = LEDC_USE_RC_FAST_CLK,
    };
    ledc_timer_config(&config);

    test_lightsleep();
}

// Lightsleep (8MD256)
TEST_CASE("Power Test: LSLP_8MD256", "[pm]")
{
#if !CONFIG_RTC_CLK_SRC_INT_8MD256
    TEST_FAIL_MESSAGE("This mode requires Kconfig option CONFIG_RTC_CLK_SRC_INT_8MD256 selected");
#endif

    test_lightsleep();
}

#if !CONFIG_RTC_CLK_SRC_INT_8MD256
// Lightsleep (with ADC/TSEN in monitor)
TEST_CASE("Power Test: LSLP_ADC_TSENS", "[pm]")
{
    esp_sleep_sub_mode_config(ESP_SLEEP_USE_ADC_TSEN_MONITOR_MODE, true);
    test_lightsleep();
    esp_sleep_sub_mode_config(ESP_SLEEP_USE_ADC_TSEN_MONITOR_MODE, false);
}

// Lightsleep (default)
TEST_CASE("Power Test: LSLP_DEFAULT", "[pm]")
{
    test_lightsleep();
}
#endif //!CONFIG_RTC_CLK_SRC_INT_8MD256
