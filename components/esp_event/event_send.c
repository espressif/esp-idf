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
#include "esp_event_legacy.h"

esp_err_t esp_event_send_noop(system_event_t *event);

extern esp_err_t esp_event_send_legacy(system_event_t *event) __attribute__((weak, alias("esp_event_send_noop")));
extern esp_err_t esp_event_send_to_default_loop(system_event_t *event) __attribute((weak, alias("esp_event_send_noop")));


esp_err_t esp_event_send_noop(system_event_t *event)
{
    return ESP_OK;
}

esp_err_t esp_event_send(system_event_t *event)
{
    // send the event to the new style event loop
    esp_err_t err = esp_event_send_to_default_loop(event);
    if (err != ESP_OK) {
        return err;
    }

    // send the event to the legacy event loop
    err = esp_event_send_legacy(event);
    if (err != ESP_OK) {
        return err;
    }

    return ESP_OK;
}
