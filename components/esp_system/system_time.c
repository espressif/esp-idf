// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_system.h"
#include "esp_attr.h"

typedef struct {
    esp_system_time_fn_t fn;        // time provider function
    uint32_t resolution;            // resolution in microseconds of the time provider
} system_time_provider_t;

// This is expected to be modified only on startup, so
// it should be safe to not put locks on it.
static system_time_provider_t s_system_time_provider;

int64_t IRAM_ATTR esp_system_get_time(void)
{
    return (*s_system_time_provider.fn)();
}

uint32_t IRAM_ATTR esp_system_get_time_resolution(void)
{
    return s_system_time_provider.resolution;
}

void esp_system_set_time_provider(esp_system_time_fn_t time_fn, uint32_t resolution)
{
    s_system_time_provider.fn = time_fn;
    s_system_time_provider.resolution = resolution;
}