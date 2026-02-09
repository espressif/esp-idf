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

#include "ot_examples_br.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_openthread.h"
#include "esp_openthread_lock.h"
#include "esp_openthread_border_router.h"
#include "protocol_examples_common.h"

#define TAG "ot_examples_br"

#if CONFIG_OPENTHREAD_CLI_WIFI
#error "CONFIG_OPENTHREAD_CLI_WIFI conflicts with the border router auto-initialization feature"
#endif

static bool s_border_router_started = false;

static void ot_br_init(void *ctx)
{
    ESP_ERROR_CHECK(example_connect());
    esp_openthread_lock_acquire(portMAX_DELAY);
    esp_openthread_set_backbone_netif(get_example_netif());
    ESP_ERROR_CHECK(esp_openthread_border_router_init());
    esp_openthread_lock_release();
    s_border_router_started = true;
    vTaskDelete(NULL);
}

esp_err_t esp_openthread_border_router_start(void)
{
    return (xTaskCreate(ot_br_init, "ot_br_init", 6144, NULL, 4, NULL) == pdPASS) ? ESP_OK : ESP_FAIL;
}

void esp_openthread_border_router_stop(void)
{
    if (s_border_router_started) {
        esp_openthread_lock_acquire(portMAX_DELAY);
        ESP_ERROR_CHECK(esp_openthread_border_router_deinit());
        esp_openthread_lock_release();
        s_border_router_started =false;
    }
}
