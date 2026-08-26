/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <esp_types.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of eFuse blocks for ESP32
 */
typedef enum {
    EFUSE_BLK0              = 0, /**< Number of eFuse block. Reserved. */

    EFUSE_BLK1              = 1, /**< Number of eFuse block. Used for Flash Encryption. If not using that Flash Encryption feature, they can be used for another purpose. */
    EFUSE_BLK_KEY0          = 1, /**< Number of eFuse block. Used for Flash Encryption. If not using that Flash Encryption feature, they can be used for another purpose. */
    EFUSE_BLK_ENCRYPT_FLASH = 1, /**< Number of eFuse block. Used for Flash Encryption. If not using that Flash Encryption feature, they can be used for another purpose. */

    EFUSE_BLK2              = 2, /**< Number of eFuse block. Used for Secure Boot. If not using that Secure Boot feature, they can be used for another purpose. */
    EFUSE_BLK_KEY1          = 2, /**< Number of eFuse block. Used for Secure Boot. If not using that Secure Boot feature, they can be used for another purpose. */
    EFUSE_BLK_SECURE_BOOT   = 2, /**< Number of eFuse block. Used for Secure Boot. If not using that Secure Boot feature, they can be used for another purpose. */

    EFUSE_BLK3              = 3, /**< Number of eFuse block. Uses for the purpose of the user. */
    EFUSE_BLK_KEY2          = 3, /**< Number of eFuse block. Uses for the purpose of the user. */
    EFUSE_BLK_KEY_MAX       = 4,

    EFUSE_BLK_MAX           = 4,
} esp_efuse_block_t;

/**
 * @brief Type of coding scheme
 */
typedef enum {
    EFUSE_CODING_SCHEME_NONE    = 0,    /**< None */
    EFUSE_CODING_SCHEME_3_4     = 1,    /**< 3/4 coding */
    EFUSE_CODING_SCHEME_REPEAT  = 2,    /**< Repeat coding */
} esp_efuse_coding_scheme_t;

/**
 * @brief Type of key purpose (virtual because ESP32 has only fixed purposes for blocks)
 */
typedef enum {
    ESP_EFUSE_KEY_PURPOSE_USER = 0,              /**< BLOCK3 */
    ESP_EFUSE_KEY_PURPOSE_SYSTEM = 1,            /**< BLOCK0 */
    ESP_EFUSE_KEY_PURPOSE_FLASH_ENCRYPTION = 2,  /**< BLOCK1 */
    ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_V2 = 3,    /**< BLOCK2 */
    ESP_EFUSE_KEY_PURPOSE_MAX,                   /**< MAX PURPOSE*/
} esp_efuse_purpose_t;

#ifdef __cplusplus
}
#endif
