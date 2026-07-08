/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Generates the SHA-256 digest of flash contents between offset and offset+length.
 *
 * Reads in MMU-sized chunks, so it handles images larger than the MMU window
 * (3.2 MB / 50 pages of 64 KB).
 *
 * @param[in]  flash_offset  Byte offset in flash.
 * @param[in]  len           Length of data in bytes.
 * @param[out] digest        32-byte buffer for the resulting SHA-256 digest.
 *
 * @return ESP_OK on success; ESP_ERR_NO_MEM if allocation fails.
 *
 * @note Defined in the esp_image_verify component; calling it in builds without
 *       that component fails at link time with an undefined reference.
 */
esp_err_t bootloader_sha256_flash_contents(uint32_t flash_offset, uint32_t len, uint8_t *digest);

#if SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384
/** @brief Generates the SHA-384 digest of flash contents between offset and offset+length.
 *
 * @param[in]  flash_offset  Byte offset in flash.
 * @param[in]  len           Length of data in bytes.
 * @param[out] digest        48-byte buffer for the resulting SHA-384 digest.
 *
 * @return ESP_OK on success; ESP_ERR_NO_MEM if allocation fails.
 *
 * @note Defined in the esp_image_verify component; calling it in builds without
 *       that component fails at link time with an undefined reference.
 */
esp_err_t bootloader_sha384_flash_contents(uint32_t flash_offset, uint32_t len, uint8_t *digest);
#endif

#ifdef __cplusplus
}
#endif
