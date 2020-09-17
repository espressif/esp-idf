// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "soc/cpu.h"  // for esp_cpu_get_ccount()
#include "esp_log.h"
#include "esp_log_private.h"


// Maximum time to wait for the mutex in a logging statement.
#define MAX_MUTEX_WAIT_MS 10
#define MAX_MUTEX_WAIT_TICKS ((MAX_MUTEX_WAIT_MS + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS)

static SemaphoreHandle_t s_log_mutex = NULL;

void esp_log_impl_lock(void)
{
    if (!s_log_mutex) {
        s_log_mutex = xSemaphoreCreateMutex();
    }
    xSemaphoreTake(s_log_mutex, portMAX_DELAY);
}

bool esp_log_impl_lock_timeout(void)
{
    if (!s_log_mutex) {
        s_log_mutex = xSemaphoreCreateMutex();
    }
    return xSemaphoreTake(s_log_mutex, MAX_MUTEX_WAIT_TICKS) == pdTRUE;
}

void esp_log_impl_unlock(void)
{
    xSemaphoreGive(s_log_mutex);
}

static bool esp_log_system_early_timestamp(char *buffer)
{
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        uint32_t timestamp = esp_log_early_timestamp();
        for (uint8_t i = 0; i < sizeof(buffer); i++) {
            if ((timestamp > 0) || (i == 0)) {
                for (uint8_t j = sizeof(buffer) - 1; j > 0; j--) {
                    buffer[j] = buffer[j - 1];
                }
                buffer[0] = (char)(timestamp % 10) + '0';
                timestamp /= 10;
            } else {
                buffer[i] = 0;
                break;
            }
        }
        return ESP_OK;
    }
    return ESP_FAIL;
}

char *esp_log_system_timestamp(void)
{
    static char buffer[13] = {0};
    static _lock_t bufferLock = 0;

    if (esp_log_system_early_timestamp(buffer) == ESP_OK) {
        return buffer;
    } else {
        struct timeval tv;
        struct tm timeinfo;

        gettimeofday(&tv, NULL);
        localtime_r(&tv.tv_sec, &timeinfo);

        _lock_acquire(&bufferLock);
        snprintf(buffer, sizeof(buffer),
                 "%02u:%02u:%02u.%03lu",
                 timeinfo.tm_hour,
                 timeinfo.tm_min,
                 timeinfo.tm_sec,
                 tv.tv_usec / 1000);
        _lock_release(&bufferLock);

        return buffer;
    }
}

char *esp_log_system_datetimestamp(void)
{
    static char buffer[24] = {0};
    static _lock_t bufferLock = 0;

    if (esp_log_system_early_timestamp(buffer) == ESP_OK) {
        return buffer;
    } else {
        struct timeval tv;
        struct tm timeinfo;

        gettimeofday(&tv, NULL);
        localtime_r(&tv.tv_sec, &timeinfo);

        _lock_acquire(&bufferLock);
        snprintf(buffer, sizeof(buffer),
                 "%04u-%02u-%02u %02u:%02u:%02u.%03lu",
                 timeinfo.tm_year+1900,
                 timeinfo.tm_mon+1,
                 timeinfo.tm_mday,
                 timeinfo.tm_hour,
                 timeinfo.tm_min,
                 timeinfo.tm_sec,
                 tv.tv_usec / 1000);
        _lock_release(&bufferLock);

        return buffer;
    }
}

uint32_t esp_log_timestamp(void)
{
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return esp_log_early_timestamp();
    }
    static uint32_t base = 0;
    if (base == 0 && xPortGetCoreID() == 0) {
        base = esp_log_early_timestamp();
    }
    TickType_t tick_count = xPortInIsrContext() ? xTaskGetTickCountFromISR() : xTaskGetTickCount();
    return base + tick_count * (1000 / configTICK_RATE_HZ);
}

/* FIXME: define an API for getting the timestamp in soc/hal */
uint32_t esp_log_early_timestamp(void)
{
    extern uint32_t g_ticks_per_us_pro;
    return esp_cpu_get_ccount() / (g_ticks_per_us_pro * 1000);
}
