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

#include "esp_check.h"
#include "esp_err.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_task_queue.h"
#include "esp_vfs.h"
#include "esp_vfs_eventfd.h"
#include "common/logging.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

static QueueHandle_t s_task_queue = NULL;
static int s_task_queue_event_fd = -1;

typedef struct {
    esp_openthread_task_t task;
    void *arg;
} task_storage_t;

esp_err_t esp_openthread_task_queue_init(void)
{
    s_task_queue_event_fd = eventfd(0, EFD_SUPPORT_ISR);
    ESP_RETURN_ON_FALSE(s_task_queue_event_fd >= 0, ESP_FAIL, OT_PLAT_LOG_TAG,
                        "Failed to create OpenThread task queue event fd");
    s_task_queue = xQueueCreate(CONFIG_OPENTHREAD_TASK_QUEUE_SIZE, sizeof(task_storage_t));
    ESP_RETURN_ON_FALSE(s_task_queue != NULL, ESP_ERR_NO_MEM, OT_PLAT_LOG_TAG,
                        "Failed to create OpenThread task queue");
    return ESP_OK;
}

esp_err_t esp_openthread_task_queue_post(esp_openthread_task_t task, void *arg)
{
    task_storage_t task_storage = {
        .task = task,
        .arg = arg,
    };
    uint64_t val = 1;
    ssize_t ret;

    ESP_RETURN_ON_FALSE(xQueueSend(s_task_queue, &task_storage, portMAX_DELAY), ESP_FAIL, OT_PLAT_LOG_TAG,
                        "Failed to post task to OpenThread task queue");
    ret = write(s_task_queue_event_fd, &val, sizeof(val));
    assert(ret == sizeof(val));

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

    return ESP_OK;
}
