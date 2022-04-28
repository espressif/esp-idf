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
#pragma once

#include "stdbool.h"
#include "esp_err.h"
#include "esp_event_base.h"
#include "bsd_strings.h"

#define ESP_EVENT_DECLARE_BASE(x)
#define ESP_EVENT_ANY_ID       (-1)

typedef void * esp_event_base_t;
typedef void * system_event_t;

const char* WIFI_EVENT;
const char* IP_EVENT;

esp_err_t esp_event_handler_register(const char * event_base, int32_t event_id, void* event_handler, void* event_handler_arg);

esp_err_t esp_event_handler_unregister(const char * event_base, int32_t event_id, void* event_handler);
