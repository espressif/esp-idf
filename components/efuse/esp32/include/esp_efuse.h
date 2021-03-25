// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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


#ifdef __cplusplus
}
#endif
