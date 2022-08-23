/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include <esp_err.h>
#include <stdint.h>

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/secure_boot.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/secure_boot.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/secure_boot.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/secure_boot.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/secure_boot.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/secure_boot.h"
#endif

#if !CONFIG_IDF_TARGET_ESP32 || CONFIG_ESP32_REV_MIN_3

/** @brief Verify the secure boot signature block for Secure Boot V2.
 *
 *  Performs RSA-PSS or ECDSA verification of the SHA-256 image based on the public key
 *  in the signature block, compared against the public key digest stored in efuse.
 *
 * Similar to esp_secure_boot_verify_signature(), but can be used when the digest is precalculated.
 * @param sig_block Pointer to signature block data
 * @param image_digest Pointer to 32 byte buffer holding SHA-256 hash.
 * @param verified_digest Pointer to 32 byte buffer that will receive verified digest if verification completes. (Used during bootloader implementation only, result is invalid otherwise.)
 *
 */
esp_err_t esp_secure_boot_verify_sbv2_signature_block(const ets_secure_boot_signature_t *sig_block, const uint8_t *image_digest, uint8_t *verified_digest);

/** @brief Legacy function to verify RSA secure boot signature block for Secure Boot V2.
 *
 * @note This is kept for backward compatibility. It internally calls esp_secure_boot_verify_sbv2_signature_block.
 *
 * @param sig_block Pointer to RSA signature block data
 * @param image_digest Pointer to 32 byte buffer holding SHA-256 hash.
 * @param verified_digest Pointer to 32 byte buffer that will receive verified digest if verification completes. (Used during bootloader implementation only, result is invalid otherwise.)
 *
 */
esp_err_t esp_secure_boot_verify_rsa_signature_block(const ets_secure_boot_signature_t *sig_block, const uint8_t *image_digest, uint8_t *verified_digest);

#endif
