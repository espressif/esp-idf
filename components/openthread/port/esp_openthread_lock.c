// Copyright 2021 Espressif Systems (Shanghai) CO LTD
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
// limitations under the License

#include "esp_openthread_lock.h"

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static SemaphoreHandle_t s_openthread_mutex = NULL;

bool esp_openthread_lock_acquire(TickType_t block_ticks)
{
    BaseType_t ret = xSemaphoreTake(s_openthread_mutex, block_ticks);
    return (ret == pdTRUE);
}

void esp_openthread_lock_release(void)
{
    xSemaphoreGive(s_openthread_mutex);
}

esp_err_t esp_openthread_lock_init(void)
{
    if (s_openthread_mutex != NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    s_openthread_mutex = xSemaphoreCreateMutex();
    if (s_openthread_mutex == NULL) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

void esp_openthread_lock_deinit(void)
{
    if (s_openthread_mutex) {
        vSemaphoreDelete(s_openthread_mutex);
        s_openthread_mutex = NULL;
    }
}
