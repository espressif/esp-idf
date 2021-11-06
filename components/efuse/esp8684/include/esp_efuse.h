/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of eFuse blocks ESP8684
 */
typedef enum {
    EFUSE_BLK0                 = 0,   /**< Number of eFuse BLOCK0. REPEAT_DATA */

    EFUSE_BLK1                 = 1,   /**< Number of eFuse BLOCK1. SYS_DATA_PART0 */
    EFUSE_BLK_SYS_DATA_PART0   = 2,   /**< Number of eFuse BLOCK2. SYS_DATA_PART0 */

    EFUSE_BLK2                 = 2,   /**< Number of eFuse BLOCK2. SYS_DATA_PART1 */
    EFUSE_BLK_SYS_DATA_PART1   = 2,   /**< Number of eFuse BLOCK2. SYS_DATA_PART1 */

    EFUSE_BLK3                 = 3,   /**< Number of eFuse BLOCK3. KEY0*/
    EFUSE_BLK_KEY0             = 3,   /**< Number of eFuse BLOCK3. KEY0*/
    EFUSE_BLK_KEY_MAX          = 4,

    EFUSE_BLK_MAX              = 4,
} esp_efuse_block_t;

/**
 * @brief Type of coding scheme
 */
typedef enum {
    EFUSE_CODING_SCHEME_NONE    = 0,    /**< None */
    EFUSE_CODING_SCHEME_RS      = 3,    /**< Reed-Solomon coding */
} esp_efuse_coding_scheme_t;

/** For ESP8684, there's no key purpose region for efuse keys, In order to maintain
  * compatibility with the previous apis, we should set the parameter of 'ets_efuse_purpose_t'
  * as default value ETS_EFUSE_KEY_PURPOSE_INVALID.
  * (In fact, this parameter can be any value, the api in the rom will not process key_purpose region)
  */
typedef enum {
    ESP_EFUSE_KEY_PURPOSE_INVALID = -1,
    ESP_EFUSE_KEY_PURPOSE_USER = 0,
    ESP_EFUSE_KEY_PURPOSE_FLASH_ENCRYPTION = 1,
    ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_V1 = 2,
} esp_efuse_purpose_t;

#ifdef __cplusplus
}
#endif
