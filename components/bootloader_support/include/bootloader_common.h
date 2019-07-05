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
#include "esp_flash_data_types.h"

/// Type of hold a GPIO in low state
typedef enum {
    GPIO_LONG_HOLD  = 1,    /*!< The long hold GPIO */
    GPIO_SHORT_HOLD = -1,   /*!< The short hold GPIO */
    GPIO_NOT_HOLD   = 0     /*!< If the GPIO input is not low */
} esp_comm_gpio_hold_t;

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

/**
 * @brief Check if the GPIO input is a long hold or a short hold.
 *
 * Number of the GPIO input will be configured as an input with internal pull-up enabled.
 * If the GPIO input is held low continuously for delay_sec period then it is a long hold.
 * If the GPIO input is held low for less period then it is a short hold.
 *
 * @param[in] num_pin Number of the GPIO input.
 * @param[in] delay_sec Input must be driven low for at least this long, continuously.
 * @return esp_comm_gpio_hold_t Defines type of hold a GPIO in low state.
 */
esp_comm_gpio_hold_t bootloader_common_check_long_hold_gpio(uint32_t num_pin, uint32_t delay_sec);

/**
 * @brief Erase the partition data that is specified in the transferred list.
 *
 * @param[in] list_erase String containing a list of cleared partitions. Like this "nvs, phy". The string must be null-terminal.
 * @param[in] ota_data_erase If true then the OTA data partition will be cleared (if there is it in partition table).
 * @return    Returns true on success, false otherwise.
 */
bool bootloader_common_erase_part_type_data(const char *list_erase, bool ota_data_erase);

/**
 * @brief Determines if the list contains the label
 *
 * @param[in] list  A string of names delimited by commas or spaces. Like this "nvs, phy, data". The string must be null-terminated.
 * @param[in] label The substring that will be searched in the list.
 * @return    Returns true if the list contains the label, false otherwise.
 */
bool bootloader_common_label_search(const char *list, char *label);
