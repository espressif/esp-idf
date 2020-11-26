// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include <stdbool.h>
#include "esp_attr.h"
#include "esp_err.h"
#ifndef BOOTLOADER_BUILD
#include "esp_spi_flash.h"
#endif
#include "soc/efuse_periph.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/* @brief Flash encryption mode based on efuse values
*/
typedef enum {
    ESP_FLASH_ENC_MODE_DISABLED,          // flash encryption is not enabled (flash crypt cnt=0)
    ESP_FLASH_ENC_MODE_DEVELOPMENT,       // flash encryption is enabled but for Development (reflash over UART allowed)
    ESP_FLASH_ENC_MODE_RELEASE            // flash encryption is enabled for Release (reflash over UART disabled)
} esp_flash_enc_mode_t;

/**
 * @file esp_partition.h
 * @brief Support functions for flash encryption features
 *
 * Can be compiled as part of app or bootloader code.
 */

/** @brief Is flash encryption currently enabled in hardware?
 *
 * Flash encryption is enabled if the FLASH_CRYPT_CNT efuse has an odd number of bits set.
 *
 * @return true if flash encryption is enabled.
 */
static inline /** @cond */ IRAM_ATTR /** @endcond */ bool esp_flash_encryption_enabled(void)
{
    uint32_t flash_crypt_cnt = 0;
#if CONFIG_IDF_TARGET_ESP32
    flash_crypt_cnt = REG_GET_FIELD(EFUSE_BLK0_RDATA0_REG, EFUSE_RD_FLASH_CRYPT_CNT);
#else
    flash_crypt_cnt = REG_GET_FIELD(EFUSE_RD_REPEAT_DATA1_REG, EFUSE_SPI_BOOT_CRYPT_CNT);
#endif
    /* __builtin_parity is in flash, so we calculate parity inline */
    bool enabled = false;
    while (flash_crypt_cnt) {
        if (flash_crypt_cnt & 1) {
            enabled = !enabled;
        }
        flash_crypt_cnt >>= 1;
    }
    return enabled;
}

/* @brief Update on-device flash encryption
 *
 * Intended to be called as part of the bootloader process if flash
 * encryption is enabled in device menuconfig.
 *
 * If FLASH_CRYPT_CNT efuse parity is 1 (ie odd number of bits set),
 * then return ESP_OK immediately (indicating flash encryption is enabled
 * and functional).
 *
 * If FLASH_CRYPT_CNT efuse parity is 0 (ie even number of bits set),
 * assume the flash has just been written with plaintext that needs encrypting.
 *
 * The following regions of flash are encrypted in place:
 *
 * - The bootloader image, if a valid plaintext image is found.[*]
 * - The partition table, if a valid plaintext table is found.
 * - Any app partition that contains a valid plaintext app image.
 * - Any other partitions with the "encrypt" flag set. [**]
 *
 * After the re-encryption process completes, a '1' bit is added to the
 * FLASH_CRYPT_CNT value (setting the parity to 1) and the EFUSE is re-burned.
 *
 * [*] If reflashing bootloader with secure boot enabled, pre-encrypt
 * the bootloader before writing it to flash or secure boot will fail.
 *
 * [**] For this reason, if serial re-flashing a previous flashed
 * device with secure boot enabled and using FLASH_CRYPT_CNT to
 * trigger re-encryption, you must simultaneously re-flash plaintext
 * content to all partitions with the "encrypt" flag set or this
 * data will be corrupted (encrypted twice).
 *
 * @note The post-condition of this function is that all
 * partitions that should be encrypted are encrypted.
 *
 * @note Take care not to power off the device while this function
 * is running, or the partition currently being encrypted will be lost.
 *
 * @note RTC_WDT will reset while encryption operations will be performed (if RTC_WDT is configured).
 *
 * @return ESP_OK if all operations succeeded, ESP_ERR_INVALID_STATE
 * if a fatal error occured during encryption of all partitions.
 */
esp_err_t esp_flash_encrypt_check_and_update(void);


/** @brief Encrypt-in-place a block of flash sectors
 *
 * @note This function resets RTC_WDT between operations with sectors.
 * @param src_addr Source offset in flash. Should be multiple of 4096 bytes.
 * @param data_length Length of data to encrypt in bytes. Will be rounded up to next multiple of 4096 bytes.
 *
 * @return ESP_OK if all operations succeeded, ESP_ERR_FLASH_OP_FAIL
 * if SPI flash fails, ESP_ERR_FLASH_OP_TIMEOUT if flash times out.
 */
esp_err_t esp_flash_encrypt_region(uint32_t src_addr, size_t data_length);

/** @brief Write protect FLASH_CRYPT_CNT
 *
 * Intended to be called as a part of boot process if flash encryption
 * is enabled but secure boot is not used. This should protect against
 * serial re-flashing of an unauthorised code in absence of secure boot.
 *
 * @note On ESP32 V3 only, write protecting FLASH_CRYPT_CNT will also prevent
 * disabling UART Download Mode. If both are wanted, call
 * esp_efuse_disable_rom_download_mode() before calling this function.
 *
 */
void esp_flash_write_protect_crypt_cnt(void);

/** @brief Return the flash encryption mode
 *
 * The API is called during boot process but can also be called by
 * application to check the current flash encryption mode of ESP32
 *
 * @return
 */
esp_flash_enc_mode_t esp_get_flash_encryption_mode(void);


/** @brief Check the flash encryption mode during startup
 *
 * @note This function is called automatically during app startup,
 * it doesn't need to be called from the app.
 *
 * Verifies the flash encryption config during startup:
 *
 * - Correct any insecure flash encryption settings if hardware
 *   Secure Boot is enabled.
 * - Log warnings if the efuse config doesn't match the project
 *  config in any way
 */
void esp_flash_encryption_init_checks(void);

#ifdef __cplusplus
}
#endif
