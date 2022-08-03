/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_custom_table.h"

// md5_digest_table 584317af6a850ee16ef6206c139d6cf5
// This file was generated from the file esp_efuse_custom_table.csv. DO NOT CHANGE THIS FILE MANUALLY.
// If you want to change some fields, you need to change esp_efuse_custom_table.csv file
// then run `efuse_common_table` or `efuse_custom_table` command it will generate this file.
// To show efuse_table run the command 'show_efuse_table'.

#define MAX_BLK_LEN CONFIG_EFUSE_MAX_BLK_LEN

// The last free bit in the block is counted over the entire file.
#define LAST_FREE_BIT_BLK3 94

_Static_assert(LAST_FREE_BIT_BLK3 <= MAX_BLK_LEN, "The eFuse table does not match the coding scheme. Edit the table and restart the efuse_common_table or efuse_custom_table command to regenerate the new files.");

static const esp_efuse_desc_t MODULE_VERSION[] = {
    {EFUSE_BLK3, 56, 8}, 	 // Module version,
};

static const esp_efuse_desc_t DEVICE_ROLE[] = {
    {EFUSE_BLK3, 64, 3}, 	 // Device role,
};

static const esp_efuse_desc_t SETTING_1[] = {
    {EFUSE_BLK3, 67, 6}, 	 // Setting 1,
};

static const esp_efuse_desc_t SETTING_2[] = {
    {EFUSE_BLK3, 73, 5}, 	 // Setting 2,
};

static const esp_efuse_desc_t CUSTOM_SECURE_VERSION[] = {
    {EFUSE_BLK3, 78, 16}, 	 // Custom secure version,
};





const esp_efuse_desc_t* ESP_EFUSE_MODULE_VERSION[] = {
    &MODULE_VERSION[0],    		// Module version
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DEVICE_ROLE[] = {
    &DEVICE_ROLE[0],    		// Device role
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SETTING_1[] = {
    &SETTING_1[0],    		// Setting 1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SETTING_2[] = {
    &SETTING_2[0],    		// Setting 2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CUSTOM_SECURE_VERSION[] = {
    &CUSTOM_SECURE_VERSION[0],    		// Custom secure version
    NULL
};
