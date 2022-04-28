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

#include <stdbool.h>
#include <stdint.h>

typedef struct esp_timer* esp_timer_handle_t;

typedef void (*esp_timer_cb_t)(void* arg);

typedef enum {
    ESP_TIMER_TASK,
} esp_timer_dispatch_t;

typedef struct {
    esp_timer_cb_t callback;        //!< Function to call when timer expires
    void* arg;                      //!< Argument to pass to the callback
    esp_timer_dispatch_t dispatch_method;   //!< Call the callback from task or from ISR
    const char* name;               //!< Timer name, used in esp_timer_dump function
    bool skip_unhandled_events;     //!< Skip unhandled events for periodic timers
} esp_timer_create_args_t;

esp_err_t esp_timer_create(const esp_timer_create_args_t* create_args,
                           esp_timer_handle_t* out_handle);
esp_err_t esp_timer_start_periodic(esp_timer_handle_t timer, uint64_t period);

esp_err_t esp_timer_stop(esp_timer_handle_t timer);

esp_err_t esp_timer_delete(esp_timer_handle_t timer);
