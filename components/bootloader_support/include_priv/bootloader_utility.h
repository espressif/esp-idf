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

#include "esp_image_format.h"

/**
 * @brief Load partition table.
 *
 * Parse partition table, get useful data such as location of
 * OTA data partition, factory app partition, and test app partition.
 *
 * @param[out] bs Bootloader state structure used to save read data.
 * @return        Return true if the partition table was succesfully loaded and MD5 checksum is valid.
 */
bool bootloader_utility_load_partition_table(bootloader_state_t* bs);

/**
 * @brief Return the index of the selected boot partition.
 *
 * This is the preferred boot partition, as determined by the partition table &
 * any OTA sequence number found in OTA data.
 * This partition will only be booted if it contains a valid app image, otherwise load_boot_image() will search
 * for a valid partition using this selection as the starting point.
 *
 * @param[in] bs Bootloader state structure.
 * @return       Returns the index on success, INVALID_INDEX otherwise.
 */
int bootloader_utility_get_selected_boot_partition(const bootloader_state_t *bs);

/**
 * @brief Load the app image for booting.
 *
 * Start from partition 'start_index', if not bootable then work backwards to FACTORY_INDEX
 * (ie try any OTA slots in descending order and then the factory partition).
 * If still nothing, start from 'start_index + 1' and work up to highest numbered OTA partition.
 * If still nothing, try TEST_APP_INDEX.
 *
 * @param[in] bs Bootloader state structure.
 * @param[in] start_index The index from which the search for images begins.
 * @param[out] result The image found.
 * @return    Returns true on success, false if there's no bootable app in the partition table.
 */
bool bootloader_utility_load_boot_image(const bootloader_state_t *bs, int start_index, esp_image_metadata_t *result);

/**
 * @brief Loading the selected image.
 *
 * Copy loaded segments to RAM, set up caches for mapped segments, and start application.
 *
 * @param[in] data Structure to hold on-flash image metadata.
 */
void bootloader_utility_load_image(const esp_image_metadata_t* image_data);
