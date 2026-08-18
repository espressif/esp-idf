/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread_task_queue.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_platform.h"
#include "esp_openthread_types.h"
#include "esp_vfs.h"
#include "esp_vfs_eventfd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/queue.h"
#include "openthread/tasklet.h"

static QueueHandle_t s_task_queue = NULL;
static QueueHandle_t s_priority_task_queue = NULL;
static int s_task_queue_event_fd = -1;
static const char *task_queue_workflow = "task_queue";

#define OT_TASK_QUEUE_SENDING_WAIT_TIME pdMS_TO_TICKS(100)

typedef struct {
    esp_openthread_task_t task;
    void *arg;
} task_storage_t;

esp_err_t esp_openthread_task_queue_init(const esp_openthread_platform_config_t *config)
{
    s_task_queue_event_fd = eventfd(0, EFD_SUPPORT_ISR);
    ESP_RETURN_ON_FALSE(s_task_queue_event_fd >= 0, ESP_FAIL, OT_PLAT_LOG_TAG,
                        "Failed to create OpenThread task queue event fd");
    s_task_queue = xQueueCreate(config->port_config.task_queue_size, sizeof(task_storage_t));
    if (s_task_queue == NULL) {
        close(s_task_queue_event_fd);
        s_task_queue_event_fd = -1;
        ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to create OpenThread task queue");
        return ESP_ERR_NO_MEM;
    }
    s_priority_task_queue = xQueueCreate(CONFIG_OPENTHREAD_PRIORITY_TASK_QUEUE_SIZE, sizeof(task_storage_t));
    if (s_priority_task_queue == NULL) {
        vQueueDelete(s_task_queue);
        s_task_queue = NULL;
        close(s_task_queue_event_fd);
        s_task_queue_event_fd = -1;
        ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to create OpenThread priority task queue");
        return ESP_ERR_NO_MEM;
    }
    return esp_openthread_platform_workflow_register(&esp_openthread_task_queue_update,
                                                     &esp_openthread_task_queue_process, task_queue_workflow);
}

void otTaskletsSignalPending(otInstance *aInstance)
{
    uint64_t val = 1;
    ssize_t ret;
    ret = write(s_task_queue_event_fd, &val, sizeof(val));
    assert(ret == sizeof(val));
}

esp_err_t IRAM_ATTR esp_openthread_task_queue_post(esp_openthread_task_t task, void *arg,
                                                    esp_openthread_task_queue_type_t type)
{
    task_storage_t task_storage = {
        .task = task,
        .arg = arg,
    };
    QueueHandle_t task_queue;
    TickType_t wait_time;
    uint64_t val = 1;
    ssize_t ret;
    BaseType_t task_woken = pdFALSE;
    BaseType_t result;

    if (type == ESP_OPENTHREAD_TASK_QUEUE_TYPE_NORMAL) {
        task_queue = s_task_queue;
        wait_time = OT_TASK_QUEUE_SENDING_WAIT_TIME;
    } else if (type == ESP_OPENTHREAD_TASK_QUEUE_TYPE_PRIO) {
        task_queue = s_priority_task_queue;
        wait_time = 0;
    } else {
        return ESP_ERR_INVALID_ARG;
    }

    if (!xPortCanYield()) {
        ESP_RETURN_ON_FALSE_ISR(task_queue != NULL && s_task_queue_event_fd >= 0, ESP_ERR_INVALID_STATE, OT_PLAT_LOG_TAG,
                                "OpenThread task queue not initialized");
        result = xQueueSendFromISR(task_queue, &task_storage, &task_woken);
        if (type == ESP_OPENTHREAD_TASK_QUEUE_TYPE_PRIO) {
            ESP_ERROR_CHECK(result == pdTRUE ? ESP_OK : ESP_FAIL);
        }
        ESP_RETURN_ON_FALSE_ISR(result == pdTRUE, ESP_FAIL, OT_PLAT_LOG_TAG,
                                "OpenThread normal task queue is full");
    } else {
        ESP_RETURN_ON_FALSE(task_queue != NULL && s_task_queue_event_fd >= 0, ESP_ERR_INVALID_STATE, OT_PLAT_LOG_TAG,
                            "OpenThread task queue not initialized");
        result = xQueueSend(task_queue, &task_storage, wait_time);
        if (type == ESP_OPENTHREAD_TASK_QUEUE_TYPE_PRIO) {
            ESP_ERROR_CHECK(result == pdTRUE ? ESP_OK : ESP_FAIL);
        }
        ESP_RETURN_ON_FALSE(result == pdTRUE, ESP_FAIL, OT_PLAT_LOG_TAG,
                            "OpenThread normal task queue is full");
    }
    ret = write(s_task_queue_event_fd, &val, sizeof(val));
    assert(ret == sizeof(val));

    if (task_woken) {
        portYIELD_FROM_ISR_NO_ARG();
    }
    return ESP_OK;

}

void esp_openthread_task_queue_update(esp_openthread_mainloop_context_t *mainloop)
{
    if (s_task_queue_event_fd >= 0) {
        FD_SET(s_task_queue_event_fd, &mainloop->read_fds);
        if (s_task_queue_event_fd > mainloop->max_fd) {
            mainloop->max_fd = s_task_queue_event_fd;
        }
    }
}

esp_err_t esp_openthread_task_queue_process(otInstance *instance, const esp_openthread_mainloop_context_t *mainloop)
{
    task_storage_t task_storage;

    if (FD_ISSET(s_task_queue_event_fd, &mainloop->read_fds)) {
        uint64_t val;
        ssize_t ret = read(s_task_queue_event_fd, &val, sizeof(val));
        assert(ret == sizeof(val));
    }

    ESP_RETURN_ON_FALSE(s_task_queue != NULL, ESP_ERR_INVALID_STATE, OT_PLAT_LOG_TAG,
                        "OpenThread task queue not initialized");
    ESP_RETURN_ON_FALSE(s_priority_task_queue != NULL, ESP_ERR_INVALID_STATE, OT_PLAT_LOG_TAG,
                        "OpenThread priority task queue not initialized");
    while (xQueueReceive(s_priority_task_queue, &task_storage, 0) == pdTRUE) {
        task_storage.task(task_storage.arg);
    }
    while (xQueueReceive(s_task_queue, &task_storage, 0) == pdTRUE) {
        task_storage.task(task_storage.arg);
    }

    return ESP_OK;
}

esp_err_t esp_openthread_task_queue_deinit(void)
{
    if (s_task_queue) {
        vQueueDelete(s_task_queue);
        s_task_queue = NULL;
    }
    if (s_priority_task_queue) {
        vQueueDelete(s_priority_task_queue);
        s_priority_task_queue = NULL;
    }
    if (s_task_queue_event_fd >= 0) {
        close(s_task_queue_event_fd);
        s_task_queue_event_fd = -1;
    }
    esp_openthread_platform_workflow_unregister(task_queue_workflow);
    return ESP_OK;
}
