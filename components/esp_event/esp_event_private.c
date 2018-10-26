// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_event_private.h"
#include "esp_event_internal.h"

#include "esp_log.h"

bool esp_event_is_handler_registered(esp_event_loop_handle_t event_loop, esp_event_base_t event_base, int32_t event_id, esp_event_handler_t event_handler)
{
    esp_event_loop_instance_t* loop = (esp_event_loop_instance_t*) event_loop;

    bool result = false;
    xSemaphoreTake(loop->mutex, portMAX_DELAY);

    esp_event_base_instance_t* base_it;
    SLIST_FOREACH(base_it, &(loop->event_bases), event_base_entry) {
        esp_event_id_instance_t* event_it;
        SLIST_FOREACH(event_it, &(base_it->event_ids), event_id_entry) {
            esp_event_handler_instance_t* handler_it;
            SLIST_FOREACH(handler_it, &(event_it->handlers), handler_entry) {
                if (base_it->base == event_base && event_it->id == event_id && handler_it->handler == event_handler) {
                    result = true;
                    goto out;
                }
            }
        }
    }

out:
    xSemaphoreGive(loop->mutex);
    return result;
}