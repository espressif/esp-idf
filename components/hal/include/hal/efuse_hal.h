/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
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

/**
 * @brief Returns the chip package version
 */
uint32_t efuse_hal_get_chip_ver_pkg(void);


#if SOC_RECOVERY_BOOTLOADER_SUPPORTED

#define EFUSE_RECOVERY_BOOTLOADER_FLASH_SECTOR_LEN (12)
#define EFUSE_RECOVERY_BOOTLOADER_ENABLED(sector) ((sector) != 0 && (sector) != ((1 << EFUSE_RECOVERY_BOOTLOADER_FLASH_SECTOR_LEN) - 1))

/**
 * @brief Returns recovery bootloader flash address
 *
 * @return Recovery bootloader flash address.
 */
uint32_t efuse_hal_get_recovery_bootloader_address(void);

/**
 * @brief Converts a recovery bootloader address to the corresponding flash sector.
 *
 * This function translates a recovery bootloader address in bytes
 * into the equivalent flash sector number.
 *
 * @param address The recovery bootloader address in bytes.
 * @return The flash sector number corresponding to the given address.
 */
uint32_t efuse_hal_convert_recovery_bootloader_address_to_flash_sectors(uint32_t address);

/**
 * @brief Returns true if recovery bootloader address is configured
 *
 * @return True - Recovery bootloader address is configured.
 */
bool efuse_hal_recovery_bootloader_enabled(void);
#endif // SOC_RECOVERY_BOOTLOADER_SUPPORTED

#ifdef __cplusplus
}
#endif
