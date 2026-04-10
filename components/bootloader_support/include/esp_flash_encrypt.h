/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_attr.h"
#include "esp_err.h"
#include "soc/soc_caps.h"
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
 * @deprecated This function is deprecated. Use esp_efuse_is_flash_encryption_enabled() instead.
 *
 * @return true if flash encryption is enabled.
 */
__attribute__((deprecated("Use esp_efuse_is_flash_encryption_enabled() instead")))
bool esp_flash_encryption_enabled(void);

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
 * if a fatal error occurred during encryption of all partitions.
 */
esp_err_t esp_flash_encrypt_check_and_update(void);

/** @brief Returns the Flash Encryption state and prints it
 *
 * @return True  - Flash Encryption is enabled
 *         False - Flash Encryption is not enabled
 */
bool esp_flash_encrypt_state(void);

/** @brief Checks if the first initialization was done
 *
 * If the first initialization was done then FLASH_CRYPT_CNT != 0
 *
 * @return true - the first initialization was done
 *         false - the first initialization was NOT done
 */
bool esp_flash_encrypt_initialized_once(void);

/** @brief The first initialization of Flash Encryption key and related eFuses
 *
 * @return ESP_OK if all operations succeeded
 */
esp_err_t esp_flash_encrypt_init(void);

/** @brief Encrypts flash content
 *
 * @return ESP_OK if all operations succeeded
 */
esp_err_t esp_flash_encrypt_contents(void);

/** @brief Activates Flash encryption on the chip
 *
 * It burns FLASH_CRYPT_CNT eFuse based on the CONFIG_SECURE_FLASH_ENCRYPTION_MODE_RELEASE option.
 *
 * @return ESP_OK if all operations succeeded
 */
esp_err_t esp_flash_encrypt_enable(void);

/** @brief Returns True if the write protection of FLASH_CRYPT_CNT is set
 *
 * @param print_error Print error if it is write protected
 *
 * @return true - if FLASH_CRYPT_CNT is write protected
 */
bool esp_flash_encrypt_is_write_protected(bool print_error);

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


#if BOOTLOADER_BUILD && CONFIG_SECURE_FLASH_ENC_ENABLED
/** @brief Set all secure eFuse features related to flash encryption
 *
 * @return
 *  - ESP_OK - On success
 */
esp_err_t esp_flash_encryption_enable_secure_features(void);

#if SOC_KEY_MANAGER_FE_KEY_DEPLOY
/** @brief Enable the key manager for flash encryption
 *
 * @return
 *  - ESP_OK - On success
 */
esp_err_t esp_flash_encryption_use_efuse_key(void);
#endif // SOC_KEY_MANAGER_FE_KEY_DEPLOY

#endif /* BOOTLOADER_BUILD && CONFIG_SECURE_FLASH_ENC_ENABLED */

/** @brief Returns the verification status for all physical security features of flash encryption in release mode
 *
 * If the device has flash encryption feature configured in the release mode,
 * then it is highly recommended to call this API in the application startup code.
 * This API verifies the sanity of the eFuse configuration against
 * the release (production) mode of the flash encryption feature.
 *
 * @return
 *  - True - all eFuses are configured correctly
 *  - False - not all eFuses are configured correctly.
 */
bool esp_flash_encryption_cfg_verify_release_mode(void);

/** @brief Switches Flash Encryption from "Development" to "Release"
 *
 * If already in "Release" mode, the function will do nothing.
 * If flash encryption efuse is not enabled yet then abort.
 * It burns:
 *  - "disable encrypt in dl mode"
 *  - set FLASH_CRYPT_CNT efuse to max
 *
 * In case of the targets that support the XTS-AES peripheral's pseudo rounds function,
 * this API would configure the pseudo rounds level efuse bit to level low if the efuse bit
 * is not set already.
 */
void esp_flash_encryption_set_release_mode(void);

#ifdef __cplusplus
}
#endif
