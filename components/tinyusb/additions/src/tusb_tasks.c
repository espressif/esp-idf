// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/usb_hal.h"
#include "tinyusb.h"
#include "tusb_tasks.h"
#include "sdkconfig.h"

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
    // Create a task for tinyusb device stack:
    xTaskCreate(tusb_device_task, "tinyUSB: main task", CFG_TUD_MAINTASK_SIZE, NULL, CONFIG_USB_TASK_PRIORITY, &s_tusb_tskh);
    if (!s_tusb_tskh) {
        return ESP_FAIL;
    } else {
        return ESP_OK;
    }
}

esp_err_t tusb_stop_task(void)
{
    if ( s_tusb_tskh != NULL ) {
        vTaskDelete(s_tusb_tskh);
    } else {
        ESP_LOGE(TAG, "tinyusb task is not started");
        return ESP_FAIL;
    }
    if (s_tusb_tskh) {
        return ESP_FAIL;
    } else {
        return ESP_OK;
    }
}
