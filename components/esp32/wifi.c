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

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#if CONFIG_WIFI_ENABLED

static bool wifi_startup_flag = false;

static wifi_startup_cb_t startup_cb;
static void *startup_ctx;

#define WIFI_DEBUG(...)
#define WIFI_API_CALL_CHECK(info, api_call, ret) \
do{\
    esp_err_t __err = (api_call);\
    if ((ret) != __err) {\
        WIFI_DEBUG("%s %d %s ret=%d\n", __FUNCTION__, __LINE__, (info), __err);\
        return __err;\
    }\
} while(0)



static void esp_wifi_task(void *pvParameters)
{
    esp_err_t err;
    wifi_init_config_t cfg;
    cfg.event_q = (xQueueHandle)esp_event_get_handler();

    do {
        err = esp_wifi_init(&cfg);
        if (err != ESP_OK) {
            WIFI_DEBUG("esp_wifi_init fail, ret=%d\n", err);
            break;
        }

        if (startup_cb) {
            err = (*startup_cb)(startup_ctx);
            if (err != ESP_OK) {
                WIFI_DEBUG("startup_cb fail, ret=%d\n", err);
                break;
            }
        }

        err = esp_wifi_start();
        if (err != ESP_OK) {
            WIFI_DEBUG("esp_wifi_start fail, ret=%d\n", err);
            break;
        }

#if CONFIG_WIFI_AUTO_CONNECT
        wifi_mode_t mode;
        bool auto_connect;
        err = esp_wifi_get_mode(&mode);
        if (err != ESP_OK) {
            WIFI_DEBUG("esp_wifi_get_mode fail, ret=%d\n", err);
        }

        err = esp_wifi_get_auto_connect(&auto_connect);
        if ((mode == WIFI_MODE_STA || mode == WIFI_MODE_APSTA) && auto_connect) {
            err = esp_wifi_connect();
            if (err != ESP_OK) {
                WIFI_DEBUG("esp_wifi_connect fail, ret=%d\n", err);
                break;
            }
        }
#endif
    } while (0);

    if (err != ESP_OK) {
        WIFI_DEBUG("wifi startup fail, deinit\n");
        esp_wifi_deinit();
    }

    vTaskDelete(NULL);
}

esp_err_t esp_wifi_startup(wifi_startup_cb_t cb, void *ctx)
{
    if (wifi_startup_flag) {
        return ESP_FAIL;
    }

    startup_cb = cb;
    startup_ctx = ctx;

    xTaskCreatePinnedToCore(esp_wifi_task, "wifiTask", ESP_TASK_WIFI_STARTUP_STACK, NULL, ESP_TASK_WIFI_STARTUP_PRIO, NULL, 0);

    return ESP_OK;
}
#endif
