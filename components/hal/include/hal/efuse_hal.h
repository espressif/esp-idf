/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief get factory mac address
 */
void efuse_hal_get_mac(uint8_t *mac);

/**
 * @brief Returns chip version
 *
 * @return Chip version in format: Major * 100 + Minor
 */
uint32_t efuse_hal_chip_revision(void);

/**
 * @brief Return block version
 *
 * @return Block version in format: Major * 100 + Minor
 */
uint32_t efuse_hal_blk_version(void);

/**
 * @brief Is flash encryption currently enabled in hardware?
 *
 * Flash encryption is enabled if the FLASH_CRYPT_CNT efuse has an odd number of bits set.
 *
 * @return true if flash encryption is enabled.
 */
bool efuse_hal_flash_encryption_enabled(void);

/**
 * @brief Returns the status of whether the bootloader (and OTA)
 *        will check the maximum chip version or not.
 *
 * @return true - Skip the maximum chip version check.
 */
bool efuse_hal_get_disable_wafer_version_major(void);

/**
 * @brief Returns the status of whether the app start-up (and OTA)
 *        will check the efuse block version or not.
 *
 * @return true - Skip the efuse block version check.
 */
bool efuse_hal_get_disable_blk_version_major(void);

/**
 * @brief Returns major chip version
 */
uint32_t efuse_hal_get_major_chip_version(void);

/**
 * @brief Returns minor chip version
 */
uint32_t efuse_hal_get_minor_chip_version(void);

#if SOC_ECDSA_SUPPORTED
/**
 * @brief Set the efuse block that should be used as ECDSA private key
 *
 * @note The efuse block must be burnt with key purpose ECDSA_KEY
 *
 * @param efuse_key_blk Efuse key block number (Must be in [EFUSE_BLK_KEY0...EFUSE_BLK_KEY_MAX - 1] range)
 */
void efuse_hal_set_ecdsa_key(int efuse_key_blk);
#endif

#ifdef __cplusplus
}
#endif
