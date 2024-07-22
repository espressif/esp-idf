/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread_lock.h"
#include "esp_openthread_common_macro.h"

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static SemaphoreHandle_t s_openthread_task_mutex = NULL;
static SemaphoreHandle_t s_openthread_mutex = NULL;

bool esp_openthread_lock_acquire(TickType_t block_ticks)
{
    ESP_RETURN_ON_FALSE(s_openthread_mutex && s_openthread_task_mutex, false, OT_PLAT_LOG_TAG,
            "Failed to acquire the lock because the mutex is not ready");
    BaseType_t ret = xSemaphoreTakeRecursive(s_openthread_mutex, block_ticks) &&
        xSemaphoreTakeRecursive(s_openthread_task_mutex, block_ticks);
    return (ret == pdTRUE);
}

void esp_openthread_lock_release(void)
{
    ESP_RETURN_ON_FALSE(s_openthread_mutex && s_openthread_task_mutex, , OT_PLAT_LOG_TAG,
            "Failed to release the lock because the mutex is not ready");
    xSemaphoreGiveRecursive(s_openthread_task_mutex);
    xSemaphoreGiveRecursive(s_openthread_mutex);
}

bool esp_openthread_task_switching_lock_acquire(TickType_t block_ticks)
{
    ESP_RETURN_ON_FALSE(s_openthread_task_mutex, false, OT_PLAT_LOG_TAG,
            "Failed to acquire the lock because the mutex is not ready");
    BaseType_t ret = xSemaphoreTakeRecursive(s_openthread_task_mutex, block_ticks);
    return (ret == pdTRUE);
}

void esp_openthread_task_switching_lock_release(void)
{
    ESP_RETURN_ON_FALSE(s_openthread_task_mutex, , OT_PLAT_LOG_TAG,
            "Failed to release the lock because the mutex is not ready");
    if (xSemaphoreGetMutexHolder(s_openthread_task_mutex) != xTaskGetCurrentTaskHandle()) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Task %s is attempting to release the OpenThread task switching lock but never acquired it.",
                pcTaskGetName(xTaskGetCurrentTaskHandle()));
        assert(false);
    }
    xSemaphoreGiveRecursive(s_openthread_task_mutex);
}

esp_err_t esp_openthread_lock_init(void)
{
    if (s_openthread_mutex != NULL || s_openthread_task_mutex != NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    s_openthread_mutex = xSemaphoreCreateRecursiveMutex();
    s_openthread_task_mutex = xSemaphoreCreateRecursiveMutex();
    if (s_openthread_mutex == NULL || s_openthread_task_mutex == NULL) {
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
    if (s_openthread_task_mutex) {
        vSemaphoreDelete(s_openthread_task_mutex);
        s_openthread_task_mutex = NULL;
    }
}
