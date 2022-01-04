/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_check.h"
#include "tinyusb.h"
#include "tusb_tasks.h"

const static char *TAG = "tusb_tsk";
static TaskHandle_t s_tusb_tskh;

/**
 * @brief This top level thread processes all usb events and invokes callbacks
 */
static void tusb_device_task(void *arg)
{
    ESP_LOGD(TAG, "tinyusb task started");
    while (1) { // RTOS forever loop
        tud_task();
    }
}

esp_err_t tusb_run_task(void)
{
    // This function is not garanteed to be thread safe, if invoked multiple times without calling `tusb_stop_task`, will cause memory leak
    // doing a sanity check anyway
    ESP_RETURN_ON_FALSE(!s_tusb_tskh, ESP_ERR_INVALID_STATE, TAG, "TinyUSB main task already started");
    // Create a task for tinyusb device stack:
    xTaskCreate(tusb_device_task, "TinyUSB", CONFIG_TINYUSB_TASK_STACK_SIZE, NULL, CONFIG_TINYUSB_TASK_PRIORITY, &s_tusb_tskh);
    ESP_RETURN_ON_FALSE(s_tusb_tskh, ESP_FAIL, TAG, "create TinyUSB main task failed");
    return ESP_OK;
}

esp_err_t tusb_stop_task(void)
{
    ESP_RETURN_ON_FALSE(s_tusb_tskh, ESP_ERR_INVALID_STATE, TAG, "TinyUSB main task not started yet");
    vTaskDelete(s_tusb_tskh);
    s_tusb_tskh = NULL;
    return ESP_OK;
}
