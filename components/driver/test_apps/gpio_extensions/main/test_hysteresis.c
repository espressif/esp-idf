/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "driver/gpio.h"


/**
 * NOTE: To run this special feature test case, a slope analog signal is needed.
 *       A simple RC circuit used here to formate pin switches to continuos slop signal.
 *
 *       +---------+
 *       |         |
 *       | (intr)26|----------------+  C:1000uF
 *       |         |                |   ++  +
 *       | (wave)27|______+-----+___+___||  |_____
 *       |         |      +-----+       ||  |     |
 *       |  ESP32  |       R:10k        ++  +     |
 *       |         |                              |
 *       |      GND|------------------------------+
 *       +---------+
 *
 *       or you can connect your slop signals from signal generator to ESP32 pin
 *       which enabled the hysteresis feature directly to have a test.
 **/


static void test_gpio_hysteresis_intr_handler(void *args)
{
    esp_rom_printf("%d\n", ++*((uint32_t *)args));
}

// This case is now tested only manually
TEST_CASE("GPIO Input hysteresis filter", "[gpio_filter][timeout=50][ignore]")
{
    const gpio_num_t TEST_HYS_IO = 26;
    const gpio_num_t TEST_WAVE_IO = 27;
    uint32_t intr_cnt=0;

    gpio_config_t gpio_cfg = {
        .pin_bit_mask = 1 << TEST_WAVE_IO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
    };
    TEST_ESP_OK(gpio_config(&gpio_cfg));

    gpio_cfg.pin_bit_mask = 1 << TEST_HYS_IO;
    gpio_cfg.mode = GPIO_MODE_INPUT;
    gpio_cfg.intr_type = GPIO_INTR_ANYEDGE;
    gpio_cfg.hys_ctrl_mode = GPIO_HYS_SOFT_ENABLE;
    TEST_ESP_OK(gpio_config(&gpio_cfg));

    gpio_install_isr_service(0);
    gpio_isr_handler_add(TEST_HYS_IO, test_gpio_hysteresis_intr_handler, &intr_cnt);

    // generate 5 rising and falling slopes to test gpio interrupt
    for (uint8_t i=0; i<5; i++) {
        printf("----falling %dth\n", i);
        gpio_set_level(TEST_WAVE_IO, 0);
        vTaskDelay(1500);
        printf("----rising  %dth\n", i);
        gpio_set_level(TEST_WAVE_IO, 1);
        vTaskDelay(1500);
    }

    gpio_isr_handler_remove(TEST_HYS_IO);
    gpio_uninstall_isr_service();
    // should shot ISR exactly 10 times
    TEST_ASSERT_UINT32_WITHIN(1, 10, intr_cnt);
}
