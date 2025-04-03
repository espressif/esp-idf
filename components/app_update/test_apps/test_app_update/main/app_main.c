/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_err.h"

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    unity_run_menu();
    ESP_ERROR_CHECK(nvs_flash_deinit());
}
