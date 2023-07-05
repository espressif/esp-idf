/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_timer.h"
#include "gpio_wakeup_example.h"

static const char *TAG = "gpio_wake_up_main";

// for test
// #define GPIO_OUTPUT_IO_0    5
// #define GPIO_OUTPUT_PIN_SEL  (1ULL<<GPIO_OUTPUT_IO_0)

esp_err_t all_config_init(void)
{
    //config gpio wake
    ESP_RETURN_ON_ERROR(example_register_gpio_wakeup_sleep(), TAG, "example_register_gpio_wakeup error");
    // auto light sleep and light sleep lock init
    ESP_RETURN_ON_ERROR(power_config(), TAG, "power_config error");

    return ESP_OK;
}

void app_main(void)
{
    // all init: light sleep、interrupt...
    ESP_ERROR_CHECK(all_config_init());
    esp_rom_printf("all init success!\n");

    //------------------ for test -----------------------------
    // //the config structure.
    // gpio_config_t io_conf = {};
    // //disable interrupt
    // io_conf.intr_type = GPIO_INTR_DISABLE;
    // //set as output mode
    // io_conf.mode = GPIO_MODE_OUTPUT;
    // //bit mask of the pins that you want to set,e.g.GPIO18/19
    // io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    // //disable pull-down mode
    // io_conf.pull_down_en = 0;
    // //disable pull-up mode
    // io_conf.pull_up_en = 0;
    // //configure GPIO with the given settings
    // gpio_config(&io_conf);


    // int cnt = 0;
    // while(cnt++ < 100) {
    //     //printf("cnt: %d\n", cnt++);
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    //     gpio_set_level(GPIO_OUTPUT_IO_0, cnt % 2);
    // }
    //------------------ for test -----------------------------


#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3

#endif
}
