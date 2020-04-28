// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_event.h>
#include <esp_wifi.h>
#include "esp_log.h"
#include "esp_wifi_internal.h"
#include "esp_debug.h"
#include "wifi_debug.h"

#define TAG "wdbg"

static bool s_debug_init = false;

#if CONFIG_ESP32_DEBUG_TASK_ENABLE
#define WIFI_DEBUG_TASK_QUEUE_SIZE 5
static SemaphoreHandle_t s_debug_mutex = NULL;
static QueueHandle_t s_debug_task_queue = NULL;
static bool s_debug_task_init = false;

void debug_lock(void)
{
    if (s_debug_mutex) {
        xSemaphoreTake(s_debug_mutex, portMAX_DELAY);
    } else {
        s_debug_mutex = xSemaphoreCreateMutex();
        if (!s_debug_mutex) {
            ESP_LOGE(TAG, "fail to create mutex");
            abort();
        } else {
            xSemaphoreTake(s_debug_mutex, portMAX_DELAY);
        }
    }
}

void debug_unlock(void)
{
    if (s_debug_mutex) {
        xSemaphoreGive(s_debug_mutex);
    } else {
        ESP_LOGE(TAG, "unlock null mutex");
        abort();
    }
}

static uint64_t debug_periodical_request(void)
{
    uint64_t request = 0;

#if CONFIG_ESP32_DEBUG_TASK_POLLING_LWIP_SOCKETS
    request |= DEBUG_MODULE_LWIP_SOCKETS;
#endif

#if CONFIG_ESP32_DEBUG_TASK_POLLING_LWIP_STATS
    request |= DEBUG_MODULE_LWIP_STATISTICS;
#endif

#if CONFIG_ESP32_DEBUG_TASK_POLLING_WIFI_HARDWARE_DIAG
    request |= DEBUG_MODULE_WIFI_HW_DIAG;
#endif

#if CONFIG_ESP32_DEBUG_TASK_POLLING_WIFI_HARDWARE_COUNTERS
    request |= DEBUG_MODULE_WIFI_HW_COUNTERS;
#endif

#if CONFIG_ESP32_DEBUG_TASK_POLLING_WIFI_HARDWARE_MISC
    request |= DEBUG_MODULE_WIFI_HW_MISC;
#endif


#if CONFIG_ESP32_DEBUG_TASK_POLLING_WIFI_INT_COUNTERS
    request |= DEBUG_MODULE_WIFI_INT_COUNTERS;
#endif

#if CONFIG_ESP32_DEBUG_TASK_POLLING_WIFI_RX_COUNTERS
    request |= DEBUG_MODULE_WIFI_RX_COUNTERS;
#endif

#if CONFIG_ESP32_DEBUG_TASK_POLLING_WIFI_TX_COUNTERS
    request |= DEBUG_MODULE_WIFI_TX_COUNTERS;
#endif

#if CONFIG_ESP32_DEBUG_TASK_POLLING_WIFI_BUFFER_COUNTERS
    request |= DEBUG_MODULE_WIFI_BUFFER_COUNTERS;
#endif

#if CONFIG_ESP32_DEBUG_TASK_POLLING_WIFI_SLEEP_COUNTERS
    request |= DEBUG_MODULE_WIFI_SLEEP_COUNTERS;
#endif

#if CONFIG_ESP32_DEBUG_TASK_POLLING_WIFI_EVENT_COUNTERS
    request |= DEBUG_MODULE_WIFI_EVENT_COUNTERS;
#endif

#if CONFIG_ESP32_DEBUG_TASK_POLLING_WIFI_NVS_INFO
    request |= DEBUG_MODULE_WIFI_NVS;
#endif

#if CONFIG_ESP32_DEBUG_TASK_POLLING_WIFI_STA_INFO
    request |= DEBUG_MODULE_WIFI_STA;
#endif

#if CONFIG_ESP32_DEBUG_TASK_POLLING_WIFI_AP_INFO
    request |= DEBUG_MODULE_WIFI_AP;
#endif

#if CONFIG_ESP32_DEBUG_TASK_POLLING_WIFI_TRC_INFO
    request |= DEBUG_MODULE_WIFI_TRC;
#endif
    return request;
}

static uint64_t debug_modules_to_lwip_modules(uint64_t modules)
{
    uint64_t lwip_modules = 0;

    if (modules & DEBUG_MODULE_LWIP_SOCKETS) {
        lwip_modules |= 1;//LWIP_MODULE_SOCKETS;
    }

    if (modules & DEBUG_MODULE_LWIP_STATS) {
        lwip_modules |= 1<<1;//LWIP_MODULE_STATS;
    }

    return lwip_modules;
}

static uint64_t debug_modules_to_wifi_modules(uint64_t modules)
{
    uint64_t wifi_modules = 0;

    if (modules & DEBUG_MODULE_WIFI_HW_DIAG) {
        wifi_modules |= WIFI_MODULE_HW_DIAG;
    }

    if (modules & DEBUG_MODULE_WIFI_HW_COUNTERS) {
        wifi_modules |= WIFI_MODULE_HW_COUNTERS;
    }

    if (modules & DEBUG_MODULE_WIFI_INT_COUNTERS) {
        wifi_modules |= WIFI_MODULE_INT_COUNTERS;
    }

    if (modules & DEBUG_MODULE_WIFI_RX_COUNTERS) {
        wifi_modules |= WIFI_MODULE_RX_COUNTERS;
    }

    if (modules & DEBUG_MODULE_WIFI_TX_COUNTERS) {
        wifi_modules |= WIFI_MODULE_TX_COUNTERS;
    }

    if (modules & DEBUG_MODULE_WIFI_BUFFER_COUNTERS) {
        wifi_modules |= WIFI_MODULE_BUFFER_COUNTERS;
    }

    if (modules & DEBUG_MODULE_WIFI_SLEEP_COUNTERS) {
        wifi_modules |= WIFI_MODULE_SLEEP;
    }

    if (modules & DEBUG_MODULE_WIFI_EVENT_COUNTERS) {
        wifi_modules |= WIFI_MODULE_EVENT_COUNTERS;
    }

    if (modules & DEBUG_MODULE_WIFI_NVS) {
        wifi_modules |= WIFI_MODULE_NVS;
    }

    if (modules & DEBUG_MODULE_WIFI_STA) {
        wifi_modules |= WIFI_MODULE_STA;
    }

    if (modules & DEBUG_MODULE_WIFI_AP) {
        wifi_modules |= WIFI_MODULE_AP;
    }

    if (modules & DEBUG_MODULE_WIFI_TRC) {
        wifi_modules |= WIFI_MODULE_TRC;
    }

    return wifi_modules;
}

static void debug_process_request(wifi_polling_request_t request)
{
    uint64_t modules = 0;

    ESP_LOGI(TAG, "poll begin, type=%llx, ram=%d mini=%d", request, esp_get_free_heap_size(), esp_get_minimum_free_heap_size());

    modules = debug_modules_to_lwip_modules(request);
    esp_lwip_dump(modules);

    modules = debug_modules_to_wifi_modules(request);
    esp_wifi_dump(modules);

    ESP_LOGI(TAG, "poll end, ram=%d mini=%d", esp_get_free_heap_size(), esp_get_minimum_free_heap_size());
}

static void debug_task(void *param)
{
    wifi_polling_request_t request = 0;
    uint32_t timeout = portMAX_DELAY;

#if CONFIG_ESP32_DEBUG_TASK_POLLING_ENABLE
    timeout = (CONFIG_ESP32_DEBUG_TASK_POLLING_PERIOD * 1000) / portTICK_RATE_MS;
#endif

    while (1) {
        if (pdTRUE != xQueueReceive(s_debug_task_queue, &request, timeout)) {
            request = debug_periodical_request();
        }

        debug_process_request(request);

        if (request == DEBUG_MODULE_TASK_DELETE) {
            break;
        }
    }

    vQueueDelete(s_debug_task_queue);
    s_debug_task_queue = NULL;
    vTaskDelete(NULL);
}

esp_err_t debug_task_create(void)
{
    int ret;

    debug_lock();
    if (s_debug_task_init == true) {
        ESP_LOGE(TAG, "wifi init task already created");
        debug_unlock();
        return ESP_OK;
    }

    s_debug_task_queue = xQueueCreate(WIFI_DEBUG_TASK_QUEUE_SIZE, sizeof(wifi_polling_request_t));
    if (!s_debug_task_queue) {
        ESP_LOGE(TAG, "fail to create debug task queue");
        debug_unlock();
        return ESP_FAIL;
    }
   
    ret = xTaskCreate(debug_task, "wifi_debug", CONFIG_ESP32_DEBUG_TASK_STACK_SIZE, NULL, CONFIG_ESP32_DEBUG_TASK_PRIORITY, NULL);
    if (pdTRUE != ret) {
        ESP_LOGE(TAG, "fail to create debug task");
        vQueueDelete(s_debug_task_queue);
        s_debug_task_queue = NULL;
        debug_unlock();
        return ESP_FAIL;
    }

    s_debug_task_init = true;
    debug_unlock();
    ESP_LOGI(TAG, "wifi debug task created");
    return ESP_OK;
}

static void debug_task_delete(void)
{
    wifi_polling_request_t request = DEBUG_MODULE_TASK_DELETE;

    debug_lock();
    if (false == s_debug_task_init) {
        ESP_LOGI(TAG, "wifi debug task already deleted");
        debug_unlock();
        return;
    }

    //post delete event to WiFi debug task with blocking
    xQueueSend(s_debug_task_queue, &request, portMAX_DELAY);

    /* Wait until debug task is deleted */
    do {
        vTaskDelay(1);
    } while (s_debug_task_queue);

    s_debug_task_init = false;
    debug_unlock();
    ESP_LOGI(TAG, "wifi debug task deleted");
    return;
}

esp_err_t esp_debug_poll(wifi_polling_request_t request)
{
    debug_lock();

    if (false == s_debug_task_init) {
        ESP_LOGE(TAG, "wifi debug task not created");
        debug_unlock();
        return ESP_FAIL;
    }

    //post event to WiFi debug task without any blocking
    if (pdTRUE != xQueueSend(s_debug_task_queue, &request, 0) ) {
        ESP_LOGW(TAG, "failed to post polling event=%llx", request);
    }

    debug_unlock();
    return ESP_OK;
}
#else
esp_err_t esp_debug_poll(wifi_polling_request_t request)
{
    ESP_LOGE(TAG, "poll disabled");
    return ESP_OK;
}
#endif

esp_err_t esp_debug_init(void)
{
    esp_err_t ret = ESP_OK;

    if (true == s_debug_init) {
        return ESP_OK;
    }

    // TODO Set Packets Filter

    ret = esp_wifi_stats_init();
    if (ESP_OK != ret) {
        return ESP_FAIL;
    }

#if CONFIG_ESP32_DEBUG_TASK_ENABLE
    //Create polling task
    ret = debug_task_create();
    if (ESP_OK != ret) {
        esp_wifi_stats_deinit();
        return ESP_FAIL;
    }
#endif
    s_debug_init = true;
    return ESP_OK;
}

esp_err_t esp_debug_deinit(void)
{
    if (false == s_debug_init) {
        return ESP_OK;
    }

    esp_wifi_stats_deinit();
#if CONFIG_ESP32_DEBUG_TASK_ENABLE
    debug_task_delete();
#endif
    s_debug_init = false;
    return ESP_OK;
}

