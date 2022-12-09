/**
 * About test environment UT_T1_GPIO:
 * Please connect GPIO18 and GPIO19
 */
#include <stdio.h>
#include <string.h>
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

#define RTCIO_CHECK(condition) TEST_ASSERT_MESSAGE((condition == ESP_OK), "ret is not ESP_OK")
#define RTCIO_VERIFY(condition, msg) TEST_ASSERT_MESSAGE((condition), msg)

#define TEST_COUNT 10
static const char *TAG = "rtcio_test";

/*
 * Test output/input function.
 */
TEST_CASE("RTCIO input/output test", "[rtcio]")
{
    ESP_LOGI(TAG, "RTCIO input/output test");
    // init rtcio
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
            RTCIO_CHECK( rtc_gpio_init(i) );
            RTCIO_CHECK( rtc_gpio_set_direction(i, RTC_GPIO_MODE_INPUT_OUTPUT) );
            RTCIO_CHECK( rtc_gpio_pullup_dis(i) );
            RTCIO_CHECK( rtc_gpio_pulldown_dis(i) );
            ESP_LOGI(TAG, "gpio %d init", i);
        }
    }

    for (int cnt = 0; cnt < TEST_COUNT; cnt++) {
        uint32_t level = cnt % 2;
        ESP_LOGI(TAG, "RTCIO output level %d", level);
        for (int i = 0; i < GPIO_PIN_COUNT; i++) {
            if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
                RTCIO_CHECK( rtc_gpio_set_level(i, level) );
                vTaskDelay(10 / portTICK_RATE_MS);
                if (rtc_gpio_get_level(i) != level) {
                    ESP_LOGE(TAG, "RTCIO input/output test err, gpio%d", i);
                }
            }
        }
        vTaskDelay(100 / portTICK_RATE_MS);
    }

    // Deinit rtcio
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
            RTCIO_CHECK( rtc_gpio_deinit(i) );
        }
    }
    ESP_LOGI(TAG, "RTCIO input/output test over");
}

/*
 * Test pullup/pulldown function.
 * Note: extern circuit should not connect.
 */
TEST_CASE("RTCIO pullup/pulldown test", "[rtcio]")
{
    ESP_LOGI(TAG, "RTCIO pullup/pulldown test");
    // init rtcio
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        int num = rtc_io_number_get(i);
        if (rtc_gpio_is_valid_gpio(i) && num > 0 && rtc_io_desc[num].pullup != 0) {
            RTCIO_CHECK( rtc_gpio_init(i) );
            RTCIO_CHECK( rtc_gpio_set_direction(i, RTC_GPIO_MODE_INPUT_ONLY) );
            RTCIO_CHECK( rtc_gpio_pullup_dis(i) );
            RTCIO_CHECK( rtc_gpio_pulldown_dis(i) );
            ESP_LOGI(TAG, "gpio %d init", i);
        }
    }

    for (int cnt = 0; cnt < TEST_COUNT; cnt++) {
        uint32_t level = cnt % 2;
        ESP_LOGI(TAG, "RTCIO pull level %d", level);
        for (int i = 0; i < GPIO_PIN_COUNT; i++) {
            int num = rtc_io_number_get(i);
            if (rtc_gpio_is_valid_gpio(i) && num > 0 && rtc_io_desc[num].pullup != 0) {
                if (level) {
                    RTCIO_CHECK( rtc_gpio_pulldown_dis(i) );
                    RTCIO_CHECK( rtc_gpio_pullup_en(i) );
                } else {
                    RTCIO_CHECK( rtc_gpio_pullup_dis(i) );
                    RTCIO_CHECK( rtc_gpio_pulldown_en(i) );
                }
                vTaskDelay(20 / portTICK_RATE_MS);
                if (rtc_gpio_get_level(i) != level) {
                    ESP_LOGE(TAG, "RTCIO pullup/pulldown test err, gpio%d", i);
                }
            }
        }
        vTaskDelay(100 / portTICK_RATE_MS);
    }

    // Deinit rtcio
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        int num = rtc_io_number_get(i);
        if (rtc_gpio_is_valid_gpio(i) && num > 0 && rtc_io_desc[num].pullup != 0) {
            RTCIO_CHECK( rtc_gpio_deinit(i) );
        }
    }
    ESP_LOGI(TAG, "RTCIO pullup/pulldown test over");
}

/*
 * Test output OD function.
 */
TEST_CASE("RTCIO output OD test", "[rtcio]")
{
    ESP_LOGI(TAG, "RTCIO output OD test");
    // init rtcio
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
            RTCIO_CHECK( rtc_gpio_init(i) );
            RTCIO_CHECK( rtc_gpio_set_direction(i, RTC_GPIO_MODE_INPUT_OUTPUT_OD) );
            RTCIO_CHECK( rtc_gpio_pullup_en(i) );
            RTCIO_CHECK( rtc_gpio_pulldown_dis(i) );
            ESP_LOGI(TAG, "gpio %d init", i);
        }
    }

    for (int cnt = 0; cnt < TEST_COUNT; cnt++) {
        uint32_t level = cnt % 2;
        ESP_LOGI(TAG, "RTCIO output level %d", level);
        for (int i = 0; i < GPIO_PIN_COUNT; i++) {
            if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
                RTCIO_CHECK( rtc_gpio_set_level(i, level) );
                vTaskDelay(10 / portTICK_RATE_MS);
                if (rtc_gpio_get_level(i) != level) {
                    ESP_LOGE(TAG, "RTCIO output OD test err, gpio%d", i);
                }
            }
        }
        vTaskDelay(100 / portTICK_RATE_MS);
    }

    // Deinit rtcio
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
            RTCIO_CHECK( rtc_gpio_deinit(i) );
        }
    }
    ESP_LOGI(TAG, "RTCIO output OD test over");
}

/*
 * Test rtcio hold function.
 */
TEST_CASE("RTCIO output hold test", "[rtcio]")
{
    ESP_LOGI(TAG, "RTCIO output hold test");
    // init rtcio
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
            RTCIO_CHECK( rtc_gpio_init(i) );
            RTCIO_CHECK( rtc_gpio_set_direction(i, RTC_GPIO_MODE_INPUT_OUTPUT_OD) );
            RTCIO_CHECK( rtc_gpio_pullup_en(i) );
            RTCIO_CHECK( rtc_gpio_pulldown_dis(i) );
            RTCIO_CHECK( rtc_gpio_set_level(i, 1) );
            ESP_LOGI(TAG, "gpio %d init, level 1", i);
        }
    }

    // hold all output rtcio.
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
            RTCIO_CHECK( rtc_gpio_hold_en(i) );
            vTaskDelay(10 / portTICK_RATE_MS);
            RTCIO_CHECK( rtc_gpio_set_level(i, 0) );
            ESP_LOGI(TAG, "RTCIO output pin hold, then set level 0");
            vTaskDelay(10 / portTICK_RATE_MS);
            if (rtc_gpio_get_level(i) == 0) {
                ESP_LOGE(TAG, "RTCIO hold test err, gpio%d", i);
            }
        }
    }

    //unhold all rtcio.
    rtc_gpio_force_hold_dis_all();

    // check the unhold status
    for (int cnt = 0; cnt < 4; cnt++) {
        uint32_t level = cnt % 2;
        ESP_LOGI(TAG, "RTCIO output level %d", level);
        for (int i = 0; i < GPIO_PIN_COUNT; i++) {
            if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
                RTCIO_CHECK( rtc_gpio_set_level(i, level) );
                vTaskDelay(10 / portTICK_RATE_MS);
                if (rtc_gpio_get_level(i) != level) {
                    ESP_LOGE(TAG, "RTCIO output OD test err, gpio%d", i);
                }
            }
        }
        vTaskDelay(100 / portTICK_RATE_MS);
    }

    // Deinit rtcio
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        if (GPIO_IS_VALID_OUTPUT_GPIO(i) && rtc_gpio_is_valid_gpio(i)) {
            RTCIO_CHECK( rtc_gpio_deinit(i) );
        }
    }
    ESP_LOGI(TAG, "RTCIO hold test over");
}