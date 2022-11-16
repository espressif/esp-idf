/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread_task_queue.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_platform.h"
#include "esp_openthread_types.h"
#include "esp_vfs.h"
#include "esp_vfs_eventfd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/queue.h"

static QueueHandle_t s_task_queue = NULL;
static int s_task_queue_event_fd = -1;
static const char *task_queue_workflow = "task_queue";

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
    ESP_RETURN_ON_FALSE(s_task_queue != NULL, ESP_ERR_NO_MEM, OT_PLAT_LOG_TAG,
                        "Failed to create OpenThread task queue");
    return esp_openthread_platform_workflow_register(&esp_openthread_task_queue_update,
                                                     &esp_openthread_task_queue_process, task_queue_workflow);
}

esp_err_t IRAM_ATTR esp_openthread_task_queue_post(esp_openthread_task_t task, void *arg)
{
    task_storage_t task_storage = {
        .task = task,
        .arg = arg,
    };
    uint64_t val = 1;
    ssize_t ret;
    BaseType_t task_woken = pdFALSE;

    if (!xPortCanYield()) {
        ESP_RETURN_ON_FALSE_ISR(xQueueSendFromISR(s_task_queue, &task_storage, &task_woken), ESP_FAIL, OT_PLAT_LOG_TAG,
                                "Failed to post task to OpenThread task queue");
    } else {
        ESP_RETURN_ON_FALSE(xQueueSend(s_task_queue, &task_storage, portMAX_DELAY), ESP_FAIL, OT_PLAT_LOG_TAG,
                            "Failed to post task to OpenThread task queue");
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
    if (s_task_queue_event_fd >= 0) {
        close(s_task_queue_event_fd);
        s_task_queue_event_fd = -1;
    }
    esp_openthread_platform_workflow_unregister(task_queue_workflow);
    return ESP_OK;
}
