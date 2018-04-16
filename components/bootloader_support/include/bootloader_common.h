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

#pragma once

/**
 * @brief Calculate crc for the OTA data partition.
 *
 * @param[in] ota_data The OTA data partition.
 * @return    Returns crc value.
 */
uint32_t bootloader_common_ota_select_crc(const esp_ota_select_entry_t *s);

/**
 * @brief Verifies the validity of the OTA data partition
 *
 * @param[in] ota_data The OTA data partition.
 * @return    Returns true on valid, false otherwise.
 */
bool bootloader_common_ota_select_valid(const esp_ota_select_entry_t *s);
