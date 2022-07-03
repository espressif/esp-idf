/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of eFuse blocks
 */
typedef enum {
    EFUSE_BLK0                 = 0,   /**< Number of eFuse BLOCK0. REPEAT_DATA */

    EFUSE_BLK1                 = 1,   /**< Number of eFuse BLOCK1. SYS_DATA_PART0 */
    EFUSE_BLK_SYS_DATA_PART0   = 2,   /**< Number of eFuse BLOCK2. SYS_DATA_PART0 */

    EFUSE_BLK2                 = 2,   /**< Number of eFuse BLOCK2. SYS_DATA_PART1 */
    EFUSE_BLK_SYS_DATA_PART1   = 2,   /**< Number of eFuse BLOCK2. SYS_DATA_PART1 */

    EFUSE_BLK3                 = 3,   /**< Number of eFuse BLOCK3. KEY0. whole block */
    EFUSE_BLK_KEY0             = 3,   /**< Number of eFuse BLOCK3. KEY0. whole block */
    EFUSE_BLK_SECURE_BOOT      = 3,
    EFUSE_BLK_KEY_MAX          = 4,

    EFUSE_BLK_MAX              = 4,   /**< Number of eFuse blocks */
} esp_efuse_block_t;

/**
 * @brief Type of coding scheme
 */
typedef enum {
    EFUSE_CODING_SCHEME_NONE    = 0,    /**< None */
    EFUSE_CODING_SCHEME_RS      = 3,    /**< Reed-Solomon coding */
} esp_efuse_coding_scheme_t;

/**
 * @brief Type of key purposes (they are virtual because this chip has only fixed purposes for block)
 */
typedef enum {
    ESP_EFUSE_KEY_PURPOSE_USER              = 0,    /**< whole BLOCK3 */
    ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY   = 1,    /**< FE uses the whole BLOCK3 (key is 256-bits) */
    ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS    = 2,    /**< FE uses lower 128-bits of BLOCK3 (key is 128-bits) */
    ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_V2    = 3,    /**< SB uses higher 128-bits of BLOCK3 (key is 128-bits) */
    ESP_EFUSE_KEY_PURPOSE_MAX,                      /**< MAX PURPOSE */
} esp_efuse_purpose_t;

#ifdef __cplusplus
}
#endif
