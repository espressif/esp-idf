/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include <esp_err.h>
#include <stdint.h>
#include "rom/secure_boot.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !CONFIG_IDF_TARGET_ESP32 || CONFIG_ESP32_REV_MIN_FULL >= 300

#if CONFIG_SECURE_BOOT_V2_ENABLED || CONFIG_SECURE_SIGNED_APPS_NO_SECURE_BOOT

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

#endif /* CONFIG_SECURE_BOOT_V2_ENABLED || CONFIG_SECURE_SIGNED_APPS_NO_SECURE_BOOT */

#endif

#ifdef __cplusplus
}
#endif
