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


esp_err_t all_config_init(void)
{
    //config gpio wake
    ESP_RETURN_ON_ERROR(example_register_gpio_wakeup(), TAG, "example_register_gpio_wakeup error");
    // auto light sleep and light sleep lock init
    ESP_RETURN_ON_ERROR(light_sleep_config(), TAG, "light_sleep_config error");

    return ESP_OK;
}

void app_main(void)
{
    // all init: light sleep、interrupt...
    ESP_ERROR_CHECK(all_config_init());
    esp_rom_printf("all init success!\n");


#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3

#endif
}
