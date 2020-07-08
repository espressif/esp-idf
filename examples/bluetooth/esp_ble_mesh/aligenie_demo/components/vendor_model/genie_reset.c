// Copyright (C) 2018-2020 Alibaba Group Holding Limited
// Adaptations to ESP-IDF Copyright (c) 2020 Espressif Systems (Shanghai) Co. Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_local_data_operation_api.h"

#include "genie_mesh.h"
#include "genie_reset.h"
#include "genie_event.h"
#include "genie_util.h"
#include "ble_mesh_example_nvs.h"

static const char *TAG  = "genie_reset";
extern nvs_handle_t NVS_HANDLE;

util_timer_t g_genie_reset_timer;

// 1:need reset 0:no reset
uint8_t g_genie_reset_flag = 0;

uint8_t genie_reset_get_flag(void)
{
    return g_genie_reset_flag;
}

/**
 * @brief Set reset flag
 *
 * @param flag
 */
static void genie_reset_set_flag(uint8_t flag)
{
    g_genie_reset_flag = flag;
}

/**
 * @brief Reset timer callback timeout, Send event to genie event to handle reset action.
 *
 * @param p_timer
 */
static void genie_reset_done_timer_cb(void *p_timer)
{
    ENTER_FUNC();
    genie_event(GENIE_EVT_HW_RESET_DONE, NULL);
}

void genie_reset_done_timer_start(void)
{
    ENTER_FUNC();
    util_timer_init(&g_genie_reset_timer, genie_reset_done_timer_cb, NULL);
    util_timer_start(&g_genie_reset_timer, GENIE_RESET_WAIT_DONE_TIMEOUT);
}

/**
 * @brief Store reboot count in flash.
 *
 * @param count
 *
 * @return esp_err_t
 */
static esp_err_t genie_reset_write_count(uint8_t count)
{
    ENTER_FUNC();
    uint8_t   data = count;
    esp_err_t ret  = ESP_OK;

    ret = ble_mesh_nvs_store(NVS_HANDLE, GENIE_STORE_RESTART_COUNT_KEY, &data, sizeof(uint8_t));
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Save the number of restarts within the set time");
    }

    return ret;
}

/**
 * @brief Read reboot count from flash.
 *
 * @param p_count
 * @return esp_err_t
 */
static esp_err_t genie_reset_read_count(uint8_t *p_count)
{
    ENTER_FUNC();
    bool      exist = false;
    esp_err_t ret   = ESP_OK;

    ret = ble_mesh_nvs_restore(NVS_HANDLE, GENIE_STORE_RESTART_COUNT_KEY, p_count, sizeof(uint8_t), &exist);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Load the number of restarts within the set time");
    }

    return ret;
}

/**
 * @brief GENIE_RESET_BY_REPEAT_TIMEOUT
 *
 * @param p_timer
 */
static void genie_reset_timer_cb(void *p_timer)
{
    ENTER_FUNC();
    uint8_t number = 0;

    genie_reset_write_count(number);
}

void genie_reset_clean_count(void)
{
    genie_reset_write_count(0);
    genie_reset_set_flag(0);
}

void genie_reset_by_repeat_init(void)
{
    ENTER_FUNC();
    uint8_t   count     = 0;
    esp_err_t flash_err = ESP_OK;

    /* we should check flash flag first */
    flash_err = genie_reset_read_count(&count);
    if (flash_err != ESP_OK) {
        ESP_LOGE(TAG, "flash read err");
        count = 0;
    }

    ESP_LOGI(TAG, "read count %d", count);

    count++;
    if (count == GENIE_RESET_BY_REPEAT_COUNTER) {
        ESP_LOGI(TAG, "Genie Event Reset By Repeat Notify");
        genie_event(GENIE_EVT_RESET_BY_REPEAT_NOTIFY, NULL);
    }

    /* update number and check if exceeded */
    if (count < GENIE_RESET_BY_REPEAT_COUNTER) {
        genie_reset_write_count(count);
        util_timer_init(&g_genie_reset_timer, genie_reset_timer_cb, NULL);
        util_timer_start(&g_genie_reset_timer, GENIE_RESET_BY_REPEAT_TIMEOUT);
    } else {
        //genie_event(GENIE_EVT_HW_RESET, NULL);
        ESP_LOGW(TAG, "genie_reset_set_flag need reset node");
        genie_reset_set_flag(1);
        util_timer_init(&g_genie_reset_timer, genie_reset_timer_cb, NULL);
        util_timer_start(&g_genie_reset_timer, GENIE_RESET_BY_REPEAT_TIMEOUT);
    }
    EXIT_FUNC();
}
