// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_err.h"
#include "esp_event.h"

const char * WIFI_EVENT = "WIFI_EVENT";
const char * IP_EVENT = "IP_EVENT";

esp_err_t esp_event_handler_register(const char * event_base, int32_t event_id, void* event_handler, void* event_handler_arg)
{
    return ESP_OK;
}

esp_err_t esp_event_handler_unregister(const char * event_base, int32_t event_id, void* event_handler)
{
    return ESP_OK;
}
