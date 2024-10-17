/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wiegand_io.h"

#define DATA_PIN0_GPIO      GPIO_NUM_0
#define DATA_PIN1_GPIO      GPIO_NUM_2

static const char *TAG = "example";
uint64_t wiegand_data = 0x5a5a5a5a;

void app_main(void)
{
    wiegand_io_handle_t wiegand_io_handle = NULL;
    wiegand_io_config_t wiegand_io_config = {
        .data_pin0 = DATA_PIN0_GPIO,
        .data_pin1 = DATA_PIN1_GPIO,
        .pulse_width_us = 50,
        .interval_width_us = 1000,
    };

    ESP_LOGI(TAG, "Configure wiegand interface");
    ESP_ERROR_CHECK(wiegand_new_io(&wiegand_io_config, &wiegand_io_handle));

    ESP_LOGI(TAG, "send wiegand data");
    ESP_ERROR_CHECK(wiegand_io_send(wiegand_io_handle, &wiegand_data, 26));
}
