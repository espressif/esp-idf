/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table_v0.0_v1.1.h"

// md5_digest_table e0c7039a210d1fb05ea0ef6585afb060
// This file was generated from the file esp_efuse_table_v0.0_v1.1.csv. DO NOT CHANGE THIS FILE MANUALLY.
// If you want to change some fields, you need to change esp_efuse_table_v0.0_v1.1.csv file
// then run `efuse_common_table` or `efuse_custom_table` command it will generate this file.
// To show efuse_table run the command 'show_efuse_table'.

static const esp_efuse_desc_t WR_DIS_ECDSA_FORCE_USE_HARDWARE_K[] = {
    {EFUSE_BLK0, 17, 1}, 	 // [] wr_dis of ECDSA_FORCE_USE_HARDWARE_K,
};

static const esp_efuse_desc_t ECDSA_FORCE_USE_HARDWARE_K[] = {
    {EFUSE_BLK0, 114, 1}, 	 // [] Represents whether hardware random number k is forced used in ESDCA. 1: force used. 0: not force used,
};





const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ECDSA_FORCE_USE_HARDWARE_K[] = {
    &WR_DIS_ECDSA_FORCE_USE_HARDWARE_K[0],    		// [] wr_dis of ECDSA_FORCE_USE_HARDWARE_K
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ECDSA_FORCE_USE_HARDWARE_K[] = {
    &ECDSA_FORCE_USE_HARDWARE_K[0],    		// [] Represents whether hardware random number k is forced used in ESDCA. 1: force used. 0: not force used
    NULL
};
