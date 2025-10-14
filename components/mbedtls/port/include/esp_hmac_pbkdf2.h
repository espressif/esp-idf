/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "esp_err.h"
#include "hal/hmac_types.h"

/**
 * @brief Derive a key using PBKDF2-HMAC-SHA256 algorithm with HMAC peripheral
 *
 * This function implements PBKDF2 (Password-Based Key Derivation Function 2) using HMAC-SHA256
 * to derive a cryptographic key from the efuse key and salt. The function uses the HMAC peripheral
 * in upstream mode to perform the underlying HMAC calculations.
 *
 * @param key_id Determines which of the 6 key blocks in the efuses should be used as the base key.
 *               The corresponding purpose field of the key block must be set to HMAC upstream purpose.
 * @param salt Input salt for the PBKDF2 algorithm
 * @param salt_len Length of the salt in bytes (must be > 0)
 * @param iterations Number of iterations (recommended >= 2048)
 * @param key_len Length of the derived key to generate in bytes (must be > 0)
 * @param[out] out_key Buffer to store the derived key. Must be at least key_len bytes.
 * @return esp_err_t
 *         - ESP_OK: Key derivation successful
 *         - ESP_ERR_INVALID_ARG: Invalid arguments (null pointers, zero lengths, or key_id out of range)
 *         - ESP_ERR_NO_MEM: Memory allocation failed
 *         - ESP_FAIL: HMAC calculation failed
 */
esp_err_t esp_hmac_derive_pbkdf2_key(hmac_key_id_t key_id, const uint8_t *salt, size_t salt_len, int iterations, size_t key_len, uint8_t *out_key);

#ifdef __cplusplus
}
#endif
