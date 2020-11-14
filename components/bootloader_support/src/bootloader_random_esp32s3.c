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
#include "sdkconfig.h"
#include "bootloader_random.h"
#include "esp_log.h"

static const char *TAG = "bootloader_random";

void bootloader_random_enable(void)
{
    ESP_LOGW(TAG, "RNG for ESP32-S3 not currently supported"); // IDF-1878
    // Don't forget to remove the following line
    // *libbootloader_support.a:bootloader_random*.*(.literal.bootloader_random_enable .text.bootloader_random_enable)
    // In the bootloader.ld when RNG support is ready for ESP32-S3
}

void bootloader_random_disable(void)
{
    ESP_LOGW(TAG, "RNG for ESP32-S3 not currently supported"); // IDF-1878
}
