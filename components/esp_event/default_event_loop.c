// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_event.h"
#include "esp_event_internal.h"
#include "esp_task.h"

/* ------------------------- Static Variables ------------------------------- */

static esp_event_loop_handle_t s_default_loop = NULL;

/* ---------------------------- Public API ---------------------------------- */

esp_err_t esp_event_handler_register(esp_event_base_t event_base, int32_t event_id,
        esp_event_handler_t event_handler, void* event_handler_arg)
{
    if (s_default_loop == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    return esp_event_handler_register_with(s_default_loop, event_base, event_id,
            event_handler, event_handler_arg);
}

esp_err_t esp_event_handler_unregister(esp_event_base_t event_base, int32_t event_id,
        esp_event_handler_t event_handler)
{
    if (s_default_loop == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    return esp_event_handler_unregister_with(s_default_loop, event_base, event_id,
            event_handler);
}

esp_err_t esp_event_post(esp_event_base_t event_base, int32_t event_id,
        void* event_data, size_t event_data_size, TickType_t ticks_to_wait)
{
    if (s_default_loop == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    return esp_event_post_to(s_default_loop, event_base, event_id,
            event_data, event_data_size, ticks_to_wait);
}


#if CONFIG_ESP_EVENT_POST_FROM_ISR
esp_err_t esp_event_isr_post(esp_event_base_t event_base, int32_t event_id,
        void* event_data, size_t event_data_size, BaseType_t* task_unblocked)
{
    if (s_default_loop == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    return esp_event_isr_post_to(s_default_loop, event_base, event_id,
            event_data, event_data_size, task_unblocked);
}
#endif


esp_err_t esp_event_loop_create_default()
{
    if (s_default_loop) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_event_loop_args_t loop_args = {
        .queue_size = CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE,
        .task_name = "sys_evt",
        .task_stack_size = ESP_TASKD_EVENT_STACK,
        .task_priority = ESP_TASKD_EVENT_PRIO,
        .task_core_id = 0
    };

    esp_err_t err;

    err = esp_event_loop_create(&loop_args, &s_default_loop);
    if (err != ESP_OK) {
        return err;
    }

    return ESP_OK;
}

esp_err_t esp_event_loop_delete_default()
{
    if (!s_default_loop) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err;

    err = esp_event_loop_delete(s_default_loop);

    if (err != ESP_OK) {
        return err;
    }

    s_default_loop = NULL;

    return ESP_OK;
}


/* Include the code to forward legacy system_event_t events to the this default
 * event loop.
 */
#include "event_send_compat.inc"
