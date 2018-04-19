// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "esp_log.h"
#include "rom/gpio.h"
#include "bootloader_config.h"
#include "bootloader_init.h"
#include "bootloader_utility.h"
#include "bootloader_common.h"
#include "sdkconfig.h"
#include "esp_image_format.h"

static const char* TAG = "boot";

static esp_err_t select_image (esp_image_metadata_t *image_data);
static int selected_boot_partition(const bootloader_state_t *bs);
/*
 * We arrive here after the ROM bootloader finished loading this second stage bootloader from flash.
 * The hardware is mostly uninitialized, flash cache is down and the app CPU is in reset.
 * We do have a stack, so we can do the initialization in C.
 */
void call_start_cpu0()
{
    // 1. Hardware initialization
    if(bootloader_init() != ESP_OK){
        return;
    }

    // 2. Select image to boot
    esp_image_metadata_t image_data;
    if(select_image(&image_data) != ESP_OK){
        return;
    }

    // 3. Loading the selected image
    bootloader_utility_load_image(&image_data);
}

// Selects image to boot
static esp_err_t select_image (esp_image_metadata_t *image_data)
{
    // 1. Load partition table
    bootloader_state_t bs = { 0 };
    if (!bootloader_utility_load_partition_table(&bs)) {
        ESP_LOGE(TAG, "load partition table error!");
        return ESP_FAIL;
    }

    // 2. Select boot partition
    int boot_index = selected_boot_partition(&bs);
    if(boot_index == INVALID_INDEX) {
        return ESP_FAIL; // Unrecoverable failure (not due to corrupt ota data or bad partition contents)
    }

    // 3. Load the app image for booting
    if (!bootloader_utility_load_boot_image(&bs, boot_index, image_data)) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

/*
 * Selects a boot partition.
 * The conditions for switching to another firmware are checked.
 */
static int selected_boot_partition(const bootloader_state_t *bs)
{
    int boot_index = bootloader_utility_get_selected_boot_partition(bs);
    if (boot_index == INVALID_INDEX) {
        return boot_index; // Unrecoverable failure (not due to corrupt ota data or bad partition contents)
    } else {
        // Check for reset to the factory firmware or for launch OTA[x] firmware.
        // Customer implementation.
        // if (gpio_pin_1 == true && ...){
        //     boot_index = required_boot_partition;
        // } ...
    }
    return boot_index;
}
