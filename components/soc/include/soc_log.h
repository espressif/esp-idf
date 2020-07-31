// Copyright 2016-2017 Espressif Systems (Shanghai) PTE LTD
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

#pragma once
#include "esp_rom_sys.h"
/**
 * @file soc_log.h
 * @brief SOC library logging functions
 *
 * To make SOC library compatible with environments which don't use ESP-IDF,
 * this header file provides wrappers for logging functions.
 */

#ifdef ESP_PLATFORM
#include "esp_log.h"
#define SOC_LOGE(tag, fmt, ...) ESP_EARLY_LOGE(tag, fmt, ##__VA_ARGS__)
#define SOC_LOGW(tag, fmt, ...) ESP_EARLY_LOGW(tag, fmt, ##__VA_ARGS__)
#define SOC_LOGI(tag, fmt, ...) ESP_EARLY_LOGI(tag, fmt, ##__VA_ARGS__)
#define SOC_LOGD(tag, fmt, ...) ESP_EARLY_LOGD(tag, fmt, ##__VA_ARGS__)
#define SOC_LOGV(tag, fmt, ...) ESP_EARLY_LOGV(tag, fmt, ##__VA_ARGS__)

#else
#include "sdkconfig.h"
#ifdef CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"  // will be removed in idf v5.0
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/ets_sys.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/ets_sys.h"
#endif

#define SOC_LOGE(tag, fmt, ...) esp_rom_printf("%s(err): " fmt, tag, ##__VA_ARGS__)
#define SOC_LOGW(tag, fmt, ...) esp_rom_printf("%s(warn): " fmt, tag, ##__VA_ARGS__)
#define SOC_LOGI(tag, fmt, ...) esp_rom_printf("%s(info): " fmt, tag, ##__VA_ARGS__)
#define SOC_LOGD(tag, fmt, ...) esp_rom_printf("%s(dbg): " fmt, tag, ##__VA_ARGS__)
#define SOC_LOGV(tag, fmt, ...) esp_rom_printf("%s: " fmt, tag, ##__VA_ARGS__)
#endif //ESP_PLATFORM
