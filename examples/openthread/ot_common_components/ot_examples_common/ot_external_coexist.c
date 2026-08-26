/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 *
 * OpenThread Command Line Example
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
*/

#include "sdkconfig.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_coexist.h"
#include "ot_examples_common.h"

void ot_external_coexist_init(void)
{
    esp_extern_coex_work_mode_t mode =
#if CONFIG_EXTERNAL_COEX_WORK_MODE_LEADER
    EXTERNAL_COEX_LEADER_ROLE;
#elif CONFIG_EXTERNAL_COEX_WORK_MODE_FOLLOWER
    EXTERNAL_COEX_FOLLOWER_ROLE;
#else
    EXTERNAL_COEX_UNKNOWN_ROLE;
#endif

    esp_external_coex_gpio_set_t gpio_pin = ESP_OPENTHREAD_DEFAULT_EXTERNAL_COEX_CONFIG();
    ESP_ERROR_CHECK(esp_external_coex_set_work_mode(mode));
    ESP_ERROR_CHECK(esp_enable_extern_coex_gpio_pin(CONFIG_EXTERNAL_COEX_WIRE_TYPE, gpio_pin));
}
