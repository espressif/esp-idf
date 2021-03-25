// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
 * @brief Type of eFuse blocks ESP32C3
 */
typedef enum {
    EFUSE_BLK0                 = 0,   /**< Number of eFuse BLOCK0. REPEAT_DATA */

    EFUSE_BLK1                 = 1,   /**< Number of eFuse BLOCK1. MAC_SPI_8M_SYS */

    EFUSE_BLK2                 = 2,   /**< Number of eFuse BLOCK2. SYS_DATA_PART1 */
    EFUSE_BLK_SYS_DATA_PART1   = 2,   /**< Number of eFuse BLOCK2. SYS_DATA_PART1 */

    EFUSE_BLK3                 = 3,   /**< Number of eFuse BLOCK3. USER_DATA*/
    EFUSE_BLK_USER_DATA        = 3,   /**< Number of eFuse BLOCK3. USER_DATA*/

    EFUSE_BLK4                 = 4,   /**< Number of eFuse BLOCK4. KEY0 */
    EFUSE_BLK_KEY0             = 4,   /**< Number of eFuse BLOCK4. KEY0 */

    EFUSE_BLK5                 = 5,   /**< Number of eFuse BLOCK5. KEY1 */
    EFUSE_BLK_KEY1             = 5,   /**< Number of eFuse BLOCK5. KEY1 */

    EFUSE_BLK6                 = 6,   /**< Number of eFuse BLOCK6. KEY2 */
    EFUSE_BLK_KEY2             = 6,   /**< Number of eFuse BLOCK6. KEY2 */

    EFUSE_BLK7                 = 7,   /**< Number of eFuse BLOCK7. KEY3 */
    EFUSE_BLK_KEY3             = 7,   /**< Number of eFuse BLOCK7. KEY3 */

    EFUSE_BLK8                 = 8,   /**< Number of eFuse BLOCK8. KEY4 */
    EFUSE_BLK_KEY4             = 8,   /**< Number of eFuse BLOCK8. KEY4 */

    EFUSE_BLK9                 = 9,   /**< Number of eFuse BLOCK9. KEY5 */
    EFUSE_BLK_KEY5             = 9,   /**< Number of eFuse BLOCK9. KEY5 */
    EFUSE_BLK_KEY_MAX          = 10,

    EFUSE_BLK10                = 10,  /**< Number of eFuse BLOCK10. SYS_DATA_PART2 */
    EFUSE_BLK_SYS_DATA_PART2   = 10,  /**< Number of eFuse BLOCK10. SYS_DATA_PART2 */

    EFUSE_BLK_MAX
} esp_efuse_block_t;

/**
 * @brief Type of coding scheme
 */
typedef enum {
    EFUSE_CODING_SCHEME_NONE    = 0,    /**< None */
    EFUSE_CODING_SCHEME_RS      = 3,    /**< Reed-Solomon coding */
} esp_efuse_coding_scheme_t;

#ifdef __cplusplus
}
#endif
