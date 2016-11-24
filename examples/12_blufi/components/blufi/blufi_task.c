// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "gki.h"
#include "bt_defs.h"
#include "bt_trace.h"
#include "bt_types.h"
#include "allocator.h"

#include "bta_api.h"
#include "bta_gatt_api.h"

#include "controller.h"

#include "hash_map.h"
#include "hash_functions.h"
#include "alarm.h"
#include "thread.h"

#include "blufi.h"
#include "blufi_adv.h"

xQueueHandle xBlufiTaskQueue;
xTaskHandle xBlufiTaskHandle;

extern void ble_server_test(void);

static void blufi_task(void *arg)
{
    BtTaskEvt_t e;

    for (;;) {
        if (pdTRUE == xQueueReceive(xBlufiTaskQueue, &e, (portTickType)portMAX_DELAY)) {
            switch (e.sig) {
            case BLUFI_SIG_SWITCH_CONTEXT:
                if (e.cb) {
                    ((BtTaskCb_t)e.cb)(e.arg);
                }
                break;
            default:
                break;
            }
        }
    }
}

static esp_err_t blufi_task_post(uint32_t sig, void *par, void *cb, void *arg)
{
    BtTaskEvt_t evt;

    evt.sig = sig;
    evt.par = par;
    evt.cb = cb;
    evt.arg = arg;

    if (xQueueSend(xBlufiTaskQueue, &evt, 10 / portTICK_RATE_MS) != pdTRUE) {
        LOG_ERROR("Blufi Post failed\n");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t blufi_transfer_context(blufi_task_cb_t cb, void *arg)
{
    LOG_DEBUG("%s cb %08x, arg %u\n", __func__, (uint32_t)cb, (uint32_t)arg);

    return blufi_task_post(BLUFI_SIG_SWITCH_CONTEXT, 0, cb, arg);
}

static void blufi_task_deinit(void)
{
    vTaskDelete(xBlufiTaskHandle);
    vQueueDelete(xBlufiTaskQueue);
}


static void blufi_task_init(void)
{
    xBlufiTaskQueue = xQueueCreate(5, sizeof(BtTaskEvt_t));
    xTaskCreate(blufi_task, "BlUFI", 4096, NULL, configMAX_PRIORITIES - 3, xBlufiTaskHandle);
}

void blufi_init(void)
{
    blufi_task_init();
    blufi_transfer_context(blufi_enable, NULL);
}

void blufi_deinit(void)
{
    blufi_transfer_context(blufi_disable, blufi_task_deinit);
}

