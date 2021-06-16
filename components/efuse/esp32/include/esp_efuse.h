/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
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


/**
 * @brief Permanently update values written to the efuse write registers
 *
 * After updating EFUSE_BLKx_WDATAx_REG registers with new values to
 * write, call this function to permanently write them to efuse.
 *
 * @note Setting bits in efuse is permanent, they cannot be unset.
 *
 * @note Due to this restriction you don't need to copy values to
 * Efuse write registers from the matching read registers, bits which
 * are set in the read register but unset in the matching write
 * register will be unchanged when new values are burned.
 *
 * @note This function is not threadsafe, if calling code updates
 * efuse values from multiple tasks then this is caller's
 * responsibility to serialise.
 *
 * @deprecated Use the batch mode instead of directly call the burn command.
 *
 * After burning new efuses, the read registers are updated to match
 * the new efuse values.
 */
void esp_efuse_burn_new_values(void) __attribute__ ((deprecated));

/* @brief Write random data to efuse key block write registers
 *
 * @note Caller is responsible for ensuring efuse
 * block is empty and not write protected, before calling.
 *
 * @note Behaviour depends on coding scheme: a 256-bit key is
 * generated and written for Coding Scheme "None", a 192-bit key
 * is generated, extended to 256-bits by the Coding Scheme,
 * and then writtten for 3/4 Coding Scheme.
 *
 * @note This function does not burn the new values, caller should
 * call esp_efuse_burn_new_values() when ready to do this.
 *
 * @deprecated Use the code below instead of this function:
 *
 * @code{c}
 * uint32_t key[8];
 * size_t key_size = 256;
 * if (coding_scheme == EFUSE_CODING_SCHEME_3_4) {
 *     key_size = 192;
 * }
 * bootloader_fill_random(key, key_size / 8);
 * esp_efuse_write_block(EFUSE_BLK1, key, 0, key_size);
 * @endcode
 *
 * @param blk_wdata0_reg Address of the first data write register
 * in the block
 */
void esp_efuse_write_random_key(uint32_t blk_wdata0_reg) __attribute__ ((deprecated));

#ifdef __cplusplus
}
#endif
