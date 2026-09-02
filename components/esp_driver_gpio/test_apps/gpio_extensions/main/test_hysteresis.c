/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"

/**
 * NOTE: To run this special feature test case, a slope analog signal is needed.
 *       A simple RC circuit used here to generate continuous slope signal.
 *
 *       +--------------+
 *       |              |
 *       | (intr)HYS_IO |----------------+  C:1000uF
 *       |              |                |   ++  +
 *       | (wave)WAVE_IO|______+-----+___+___||  |_____
 *       |              |      +-----+       ||  |     |
 *       |     ESP32    |       R:10k        ++  +     |
 *       |              |                              |
 *       |       GND_IO |------------------------------+
 *       +--------------+
 *
 *       or you can connect your slop signals from signal generator to ESP32 pin
 *       which enabled the hysteresis feature directly to have a test.
 **/

#define TEST_GPIO_HYS_IO    4
#define TEST_GPIO_WAVE_IO   5
#define TEST_GPIO_GND_IO    10  // The pin alwayse output 0 to use as near GND

static int test_count_io_glitch_in_loop(int (*get_level_func)(gpio_num_t), gpio_num_t glitch_io, int loop_cnt)
{
    int glitch_cnt = 0;
    int init_level = get_level_func(glitch_io);
    while (init_level == get_level_func(glitch_io));
    for (int i = 0; i < loop_cnt; i++) {
        if (init_level != get_level_func(glitch_io)) {
            init_level = !init_level;
            glitch_cnt++;
        }
    }
    return glitch_cnt;
}

static void test_check_glitch_result(int glitch_cnt, bool hys_en)
{
    if (hys_en) {
        TEST_ASSERT_EQUAL(1, glitch_cnt); // must only 1 edge if hysteresis is enabled
    } else {
        TEST_ASSERT_GREATER_THAN(10, glitch_cnt);
    }
}

// This case is now tested only manually
TEST_CASE("GPIO Input hysteresis filter", "[gpio_filter][timeout=50][ignore]")
{
    // prepare for wave and ground io
    gpio_config_t gpio_cfg = {
        .pin_bit_mask = BIT64(TEST_GPIO_WAVE_IO) | BIT64(TEST_GPIO_GND_IO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
    };
    TEST_ESP_OK(gpio_config(&gpio_cfg));
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_GND_IO, 0));

    // prepare for hysteresis io
    gpio_cfg.pin_bit_mask = BIT64(TEST_GPIO_HYS_IO);
    gpio_cfg.mode = GPIO_MODE_INPUT;

    printf("Initial level ...\n");
    gpio_set_level(TEST_GPIO_WAVE_IO, 0);
    vTaskDelay(3000);

    for (int hys_en = 0; hys_en < 2; hys_en++) {
        printf("Test with hysteresis %s\n", hys_en ? "enabled" : "disabled");
        gpio_cfg.hys_ctrl_mode = hys_en ? GPIO_HYS_SOFT_ENABLE : GPIO_HYS_SOFT_DISABLE;
        TEST_ESP_OK(gpio_config(&gpio_cfg));

        // generate 3 rising and falling slopes to test gpio interrupt
        for (uint8_t i = 0; i < 3; i++) {
            printf("----rising  %dth: ", i);
            gpio_set_level(TEST_GPIO_WAVE_IO, 1);
            int glitch_cnt = test_count_io_glitch_in_loop(gpio_get_level, TEST_GPIO_HYS_IO, 1000000);
            printf("glitch_cnt: %d\n", glitch_cnt);
            test_check_glitch_result(glitch_cnt, hys_en);

            printf("----falling %dth: ", i);
            gpio_set_level(TEST_GPIO_WAVE_IO, 0);
            glitch_cnt = test_count_io_glitch_in_loop(gpio_get_level, TEST_GPIO_HYS_IO, 1000000);
            printf("glitch_cnt: %d\n", glitch_cnt);
            test_check_glitch_result(glitch_cnt, hys_en);
        }

    }
    TEST_ESP_OK(gpio_reset_pin(TEST_GPIO_HYS_IO));
    TEST_ESP_OK(gpio_reset_pin(TEST_GPIO_WAVE_IO));
    TEST_ESP_OK(gpio_reset_pin(TEST_GPIO_GND_IO));
}

#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
TEST_CASE("RTCIO Input hysteresis filter", "[gpio_filter][timeout=50][ignore]")
{
    // prepare for wave and ground io
    gpio_config_t gpio_cfg = {
        .pin_bit_mask = BIT64(TEST_GPIO_WAVE_IO) | BIT64(TEST_GPIO_GND_IO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
    };
    TEST_ESP_OK(gpio_config(&gpio_cfg));
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_GND_IO, 0));

    // prepare for hysteresis io
    TEST_ESP_OK(rtc_gpio_init(TEST_GPIO_HYS_IO));
    TEST_ESP_OK(rtc_gpio_set_direction(TEST_GPIO_HYS_IO, RTC_GPIO_MODE_INPUT_ONLY));
    int (*io_get_level)(gpio_num_t) = (int (*)(gpio_num_t)) rtc_gpio_get_level;

    printf("Initial level ...\n");
    gpio_set_level(TEST_GPIO_WAVE_IO, 0);
    vTaskDelay(3000);

    for (int hys_en = 0; hys_en < 2; hys_en++) {
        printf("Test with hysteresis %s\n", hys_en ? "enabled" : "disabled");
        hys_en ? rtc_gpio_hysteresis_en(TEST_GPIO_HYS_IO) : rtc_gpio_hysteresis_dis(TEST_GPIO_HYS_IO);

        // generate 3 rising and falling slopes to test gpio interrupt
        for (uint8_t i = 0; i < 3; i++) {
            printf("----rising  %dth: ", i);
            gpio_set_level(TEST_GPIO_WAVE_IO, 1);
            int glitch_cnt = test_count_io_glitch_in_loop(io_get_level, TEST_GPIO_HYS_IO, 1000000);
            printf("glitch_cnt: %d\n", glitch_cnt);
            test_check_glitch_result(glitch_cnt, hys_en);

            printf("----falling %dth: ", i);
            gpio_set_level(TEST_GPIO_WAVE_IO, 0);
            glitch_cnt = test_count_io_glitch_in_loop(io_get_level, TEST_GPIO_HYS_IO, 1000000);
            printf("glitch_cnt: %d\n", glitch_cnt);
            test_check_glitch_result(glitch_cnt, hys_en);
        }
    }
    TEST_ESP_OK(rtc_gpio_deinit(TEST_GPIO_HYS_IO));
    TEST_ESP_OK(gpio_reset_pin(TEST_GPIO_WAVE_IO));
    TEST_ESP_OK(gpio_reset_pin(TEST_GPIO_GND_IO));
}
#endif
