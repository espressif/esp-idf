/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "driver/gpio.h"
#include "ulp_lp_core.h"
#include "hal/lp_core_ll.h"
#include "lp_core_test_app_gpio_wakeup.h"

#define TEST_WAKEUP_PIN CONFIG_TEST_LP_GPIO_WAKEUP_PIN
#define GPIO_IDLE_HIGH_SIGNAL "gpio idle high"

extern const uint8_t lp_core_gpio_wakeup_bin_start[] asm("_binary_lp_core_test_app_gpio_wakeup_bin_start");
extern const uint8_t lp_core_gpio_wakeup_bin_end[]   asm("_binary_lp_core_test_app_gpio_wakeup_bin_end");

static void configure_wakee_gpio(void)
{
    rtc_gpio_init(TEST_WAKEUP_PIN);
    rtc_gpio_set_direction(TEST_WAKEUP_PIN, RTC_GPIO_MODE_INPUT_ONLY);
    rtc_gpio_pulldown_dis(TEST_WAKEUP_PIN);
    rtc_gpio_pullup_en(TEST_WAKEUP_PIN);
    rtc_gpio_wakeup_enable(TEST_WAKEUP_PIN, GPIO_INTR_NEGEDGE);
}

static void load_and_start_lp_core_program(void)
{
    TEST_ESP_OK(ulp_lp_core_load_binary(lp_core_gpio_wakeup_bin_start,
                                        lp_core_gpio_wakeup_bin_end - lp_core_gpio_wakeup_bin_start));

    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_IO,
    };

    TEST_ESP_OK(ulp_lp_core_run(&cfg));
}

static void configure_waker_gpio_output(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << TEST_WAKEUP_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    TEST_ESP_OK(gpio_config(&io_conf));
}

static void lp_gpio_wakeup_wakee_init(void)
{
    configure_wakee_gpio();
    load_and_start_lp_core_program();

    unity_wait_for_signal(GPIO_IDLE_HIGH_SIGNAL);

    TEST_ESP_OK(esp_sleep_enable_ulp_wakeup());
    esp_deep_sleep_start();
    TEST_FAIL_MESSAGE("Should have entered deep sleep");
}

static void lp_gpio_wakeup_wakee_after_wake(void)
{
    TEST_ASSERT(esp_sleep_get_wakeup_causes() & BIT(ESP_SLEEP_WAKEUP_ULP));
    printf("LP wakeup cause: 0x%" PRIx32 "\n", ulp_lp_wakeup_cause);
    TEST_ASSERT_EQUAL_HEX32(LP_CORE_LL_WAKEUP_SOURCE_LP_IO, ulp_lp_wakeup_cause);
}

static void lp_gpio_wakeup_waker_idle_high(void)
{
    configure_waker_gpio_output();
    TEST_ESP_OK(gpio_set_level(TEST_WAKEUP_PIN, 1));
}

static void lp_gpio_wakeup_waker_wake_low(void)
{
    configure_waker_gpio_output();
    TEST_ESP_OK(gpio_set_level(TEST_WAKEUP_PIN, 0));
}

TEST_CASE_MULTIPLE_STAGES(
    "LP GPIO wakeup wakee from deep sleep",
    "[lp_core][lp_gpio_wakeup][test_env=generic_multi_device][timeout=120][deepsleep]",
    lp_gpio_wakeup_wakee_init,
    lp_gpio_wakeup_wakee_after_wake);

TEST_CASE(
    "LP GPIO wakeup waker set idle high",
    "[lp_core][lp_gpio_wakeup][test_env=generic_multi_device][timeout=120]")
{
    lp_gpio_wakeup_waker_idle_high();
}

TEST_CASE(
    "LP GPIO wakeup waker set wake low",
    "[lp_core][lp_gpio_wakeup][test_env=generic_multi_device][timeout=120]")
{
    lp_gpio_wakeup_waker_wake_low();
}
