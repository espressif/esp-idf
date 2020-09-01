// Copyright 2010-2018 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_log.h"

// platform related stuff

#define HAL_SWAP32(word) __builtin_bswap32(word)
#define HAL_SWAP64(word) __builtin_bswap64(word)

#define HAL_LOGE(...)    ESP_LOGE(__VA_ARGS__)
#define HAL_LOGW(...)    ESP_LOGW(__VA_ARGS__)
#define HAL_LOGI(...)    ESP_LOGI(__VA_ARGS__)
#define HAL_LOGD(...)    ESP_LOGD(__VA_ARGS__)
#define HAL_LOGV(...)    ESP_LOGV(__VA_ARGS__)

#define STATIC_HAL_REG_CHECK(TAG, ENUM, VAL) _Static_assert((ENUM) == (VAL), #TAG" "#ENUM" definition no longer matches register value")
