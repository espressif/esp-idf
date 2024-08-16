/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "test_rtcio.h"
#include "esp_system.h"
#include "esp_sleep.h"
#include "unity.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "esp_log.h"
#include "soc/rtc_io_periph.h"
#include "soc/soc_caps.h"

#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
static const char *TAG = "rtcio_test";

#define RTCIO_CHECK(condition) TEST_ASSERT_MESSAGE((condition == ESP_OK), "ret is not ESP_OK")

#define TEST_COUNT 10

/*
 * Test output/input function.
 */
TEST_CASE("RTCIO_input/output_test", "[rtcio]")
{
    ESP_LOGI(TAG, "RTCIO input/output test");

    // init rtcio
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
            RTCIO_CHECK(rtc_gpio_init(i));
            RTCIO_CHECK(rtc_gpio_set_direction(i, RTC_GPIO_MODE_INPUT_OUTPUT));
            RTCIO_CHECK(rtc_gpio_pullup_dis(i));
            RTCIO_CHECK(rtc_gpio_pulldown_dis(i));
            ESP_LOGI(TAG, "gpio %d init", i);
        }
    }

    for (int cnt = 0; cnt < TEST_COUNT; cnt++) {
        int level = cnt % 2;
        ESP_LOGI(TAG, "RTCIO output level %d", level);
        for (int i = 0; i < GPIO_PIN_COUNT; i++) {
            if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
                RTCIO_CHECK(rtc_gpio_set_level(i, level));
                vTaskDelay(10 / portTICK_PERIOD_MS);
                if (rtc_gpio_get_level(i) != level) {
                    ESP_LOGE(TAG, "RTCIO input/output test err, gpio%d", i);
                    TEST_FAIL();
                }
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    // Deinit rtcio
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
            RTCIO_CHECK(rtc_gpio_deinit(i));
        }
    }
    ESP_LOGI(TAG, "RTCIO input/output test over");
}

/*
 * Test pullup/pulldown function.
 * Note: extern circuit should not connect.
 */
TEST_CASE("RTCIO_pullup/pulldown_test", "[rtcio]")
{
    ESP_LOGI(TAG, "RTCIO pullup/pulldown test");

    // init rtcio
    for (int i = 0; i < TEST_GPIO_PIN_COUNT; i++) {
        int num = rtc_io_number_get(s_test_map[i]);
        if (rtc_gpio_is_valid_gpio(s_test_map[i]) && num > 0 && RTCIO_SUPPORT_PU_PD(num)) {
            RTCIO_CHECK(rtc_gpio_init(s_test_map[i]));
            RTCIO_CHECK(rtc_gpio_set_direction(s_test_map[i], RTC_GPIO_MODE_INPUT_ONLY));
            RTCIO_CHECK(rtc_gpio_pullup_dis(s_test_map[i]));
            RTCIO_CHECK(rtc_gpio_pulldown_dis(s_test_map[i]));
            ESP_LOGI(TAG, "gpio %d init", s_test_map[i]);
        }
    }

    for (int cnt = 0; cnt < TEST_COUNT; cnt++) {
        int level = cnt % 2;
        ESP_LOGI(TAG, "RTCIO pull level %d", level);
        for (int i = 0; i < TEST_GPIO_PIN_COUNT; i++) {
            int num = rtc_io_number_get(s_test_map[i]);
            if (rtc_gpio_is_valid_gpio(s_test_map[i]) && num > 0 && RTCIO_SUPPORT_PU_PD(num)) {
                if (level) {
                    RTCIO_CHECK(rtc_gpio_pulldown_dis(s_test_map[i]));
                    RTCIO_CHECK(rtc_gpio_pullup_en(s_test_map[i]));
                } else {
                    RTCIO_CHECK(rtc_gpio_pullup_dis(s_test_map[i]));
                    RTCIO_CHECK(rtc_gpio_pulldown_en(s_test_map[i]));
                }
                vTaskDelay(20 / portTICK_PERIOD_MS);
                if (rtc_gpio_get_level(s_test_map[i]) != level) {
                    ESP_LOGE(TAG, "RTCIO pullup/pulldown test err, gpio%d", s_test_map[i]);
                    TEST_FAIL();
                }
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    // Deinit rtcio
    for (int i = 0; i < TEST_GPIO_PIN_COUNT; i++) {
        int num = rtc_io_number_get(s_test_map[i]);
        if (rtc_gpio_is_valid_gpio(s_test_map[i]) && num > 0 && RTCIO_SUPPORT_PU_PD(num)) {
            RTCIO_CHECK(rtc_gpio_deinit(s_test_map[i]));
        }
    }
    ESP_LOGI(TAG, "RTCIO pullup/pulldown test over");
}

/*
 * Test output OD function.
 */
TEST_CASE("RTCIO_output_OD_test", "[rtcio]")
{
    ESP_LOGI(TAG, "RTCIO output OD test");

    // init rtcio
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
            RTCIO_CHECK(rtc_gpio_init(i));
            RTCIO_CHECK(rtc_gpio_set_direction(i, RTC_GPIO_MODE_INPUT_OUTPUT_OD));
            RTCIO_CHECK(rtc_gpio_pullup_en(i));
            RTCIO_CHECK(rtc_gpio_pulldown_dis(i));
            ESP_LOGI(TAG, "gpio %d init", i);
        }
    }

    for (int cnt = 0; cnt < TEST_COUNT; cnt++) {
        int level = cnt % 2;
        ESP_LOGI(TAG, "RTCIO output level %d", level);
        for (int i = 0; i < GPIO_PIN_COUNT; i++) {
            if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
                RTCIO_CHECK(rtc_gpio_set_level(i, level));
                vTaskDelay(10 / portTICK_PERIOD_MS);
                if (rtc_gpio_get_level(i) != level) {
                    ESP_LOGE(TAG, "RTCIO output OD test err, gpio%d", i);
                    TEST_FAIL();
                }
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    // Deinit rtcio
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
            RTCIO_CHECK(rtc_gpio_deinit(i));
        }
    }
    ESP_LOGI(TAG, "RTCIO output OD test over");
}

#if SOC_RTCIO_HOLD_SUPPORTED
/*
 * Test rtcio hold function.
 */
TEST_CASE("RTCIO_output_hold_test", "[rtcio]")
{
    ESP_LOGI(TAG, "RTCIO output hold test");

    // init rtcio
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
            RTCIO_CHECK(rtc_gpio_init(i));
            RTCIO_CHECK(rtc_gpio_set_direction(i, RTC_GPIO_MODE_INPUT_OUTPUT_OD));
            RTCIO_CHECK(rtc_gpio_pullup_en(i));
            RTCIO_CHECK(rtc_gpio_pulldown_dis(i));
            RTCIO_CHECK(rtc_gpio_set_level(i, 1));
            ESP_LOGI(TAG, "gpio %d init, level 1", i);
        }
    }

    // hold all output rtcio.
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
            RTCIO_CHECK(rtc_gpio_hold_en(i));
            vTaskDelay(10 / portTICK_PERIOD_MS);
            RTCIO_CHECK(rtc_gpio_set_level(i, 0));
            ESP_LOGI(TAG, "RTCIO output pin hold, then set level 0");
            vTaskDelay(10 / portTICK_PERIOD_MS);
            if (rtc_gpio_get_level(i) == 0) {
                ESP_LOGE(TAG, "RTCIO hold test err, gpio%d", i);
                TEST_FAIL();
            }
        }
    }

    // unhold all rtcio.
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
            RTCIO_CHECK(rtc_gpio_hold_dis(i));
        }
    }

    // check the unhold status
    for (int cnt = 0; cnt < 4; cnt++) {
        int level = cnt % 2;
        ESP_LOGI(TAG, "RTCIO output level %d", level);
        for (int i = 0; i < GPIO_PIN_COUNT; i++) {
            if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
                RTCIO_CHECK(rtc_gpio_set_level(i, level));
                vTaskDelay(10 / portTICK_PERIOD_MS);
                if (rtc_gpio_get_level(i) != level) {
                    ESP_LOGE(TAG, "RTCIO output OD test err, gpio%d", i);
                    TEST_FAIL();
                }
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    // Deinit rtcio
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
            RTCIO_CHECK(rtc_gpio_deinit(i));
        }
    }
    ESP_LOGI(TAG, "RTCIO hold test over");
}
#endif //SOC_RTCIO_HOLD_SUPPORTED
#endif //SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

#if SOC_DEEP_SLEEP_SUPPORTED && SOC_GPIO_SUPPORT_HOLD_IO_IN_DSLP
// It is not necessary to test every rtcio pin, it will take too much ci testing time for deep sleep
// Only tests on s_test_map[TEST_RTCIO_DEEP_SLEEP_PIN_INDEX] pin
// (ESP32: IO25, ESP32S2, S3: IO6, C6: IO5, H2: IO12, C5: IO5) these pads' default configuration is low level
#define TEST_RTCIO_DEEP_SLEEP_PIN_INDEX 5

static void rtcio_deep_sleep_hold_test_first_stage(void)
{
    printf("configure rtcio pin to hold during deep sleep");
    int io_num = s_test_map[TEST_RTCIO_DEEP_SLEEP_PIN_INDEX];

    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(2000000));

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = (1ULL << io_num),
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    gpio_config(&io_conf);

    gpio_set_level(io_num, 1);
    // Enable global persistence
    TEST_ESP_OK(gpio_hold_en(io_num));

    esp_deep_sleep_start();
}

static void rtcio_deep_sleep_hold_test_second_stage(void)
{
    int io_num = s_test_map[TEST_RTCIO_DEEP_SLEEP_PIN_INDEX];
    // Check reset reason is waking up from deepsleep
    TEST_ASSERT_EQUAL(ESP_RST_DEEPSLEEP, esp_reset_reason());
    // Pin should stay at high level after the deep sleep
    TEST_ASSERT_EQUAL_INT(1, gpio_get_level(io_num));

    gpio_hold_dis(io_num);
}

/*
 * Test rtcio hold function during deep sleep.
 * This test case can only check the hold state after waking up from deep sleep
 * If you want to check that the rtcio hold function works properly during deep sleep,
 * please use logic analyzer or oscilloscope
 */
TEST_CASE_MULTIPLE_STAGES("RTCIO_deep_sleep_output_hold_test", "[rtcio]",
                          rtcio_deep_sleep_hold_test_first_stage,
                          rtcio_deep_sleep_hold_test_second_stage)
#endif // SOC_DEEP_SLEEP_SUPPORTED && SOC_GPIO_SUPPORT_HOLD_IO_IN_DSLP
