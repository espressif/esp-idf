// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __ESP_OTA_SELECT_H
#define __ESP_OTA_SELECT_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* OTA selection structure (two copies in the OTA data partition.)
   Size of 32 bytes is friendly to flash encryption */
typedef struct {
    uint32_t seq;
    uint32_t boot_app_subtype;
    uint8_t  reserved[20];
    uint32_t crc; /* CRC32 is computed with crc field set to 0. */
} esp_ota_select_entry_t;

uint32_t esp_ota_select_crc(const esp_ota_select_entry_t *s);
bool esp_ota_select_valid(const esp_ota_select_entry_t *s);
const esp_ota_select_entry_t *esp_ota_choose_current(const esp_ota_select_entry_t s[2]);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_OTA_SELECT_H */
