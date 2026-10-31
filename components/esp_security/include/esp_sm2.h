/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "soc/soc_caps.h"

#if SOC_ECC_SUPPORT_CURVE_SM2 && SOC_ECC_EXTENDED_MODES_SUPPORTED

#ifdef __cplusplus
extern "C" {
#endif

/** Length in bytes of every SM2 parameter (coordinate, scalar, digest). */
#define ESP_SM2_PARAM_LEN 32

/**
 * @brief Derive the SM2 public key from a private key.
 *
 * All buffers are big-endian and ESP_SM2_PARAM_LEN bytes long.
 *
 * @param priv_key Private key d. Must satisfy 1 <= d <= n-2.
 * @param pub_x    Output: public key X coordinate.
 * @param pub_y    Output: public key Y coordinate.
 *
 * @return ESP_OK on success. ESP_ERR_INVALID_ARG on a NULL pointer or an
 *         out-of-range key. ESP_ERR_NOT_SUPPORTED when the DIS_SM_CRYPT
 *         eFuse disables SM crypto. ESP_FAIL is reserved for a hardware
 *         error. The ECC peripheral cannot report one for this operation,
 *         so this function does not return ESP_FAIL today.
 */
esp_err_t esp_sm2_get_pubkey(const uint8_t priv_key[ESP_SM2_PARAM_LEN],
                             uint8_t pub_x[ESP_SM2_PARAM_LEN],
                             uint8_t pub_y[ESP_SM2_PARAM_LEN]);

/**
 * @brief Generate an SM2 signature (GB/T 32918.2) over a digest.
 *
 * The caller computes e = SM3(ZA || M). All buffers are big-endian and
 * ESP_SM2_PARAM_LEN bytes long.
 *
 * @note This function gets the signature nonce from esp_fill_random().
 *       Make sure that a true entropy source is available before you sign.
 *       Refer to the ESP-IDF documentation about random number generation.
 *       A biased nonce lets an attacker calculate the private key.
 *
 * @param digest   Digest e to sign.
 * @param priv_key Private key d. Must satisfy 1 <= d <= n-2.
 * @param sig_r    Output: signature component r.
 * @param sig_s    Output: signature component s.
 *
 * @return ESP_OK on success. ESP_ERR_INVALID_ARG on a NULL pointer or an
 *         out-of-range key. ESP_ERR_NOT_SUPPORTED when the DIS_SM_CRYPT
 *         eFuse disables SM crypto. ESP_FAIL when the function used all of
 *         its retries. Only a failed random number generator causes this.
 */
esp_err_t esp_sm2_sign(const uint8_t digest[ESP_SM2_PARAM_LEN],
                       const uint8_t priv_key[ESP_SM2_PARAM_LEN],
                       uint8_t sig_r[ESP_SM2_PARAM_LEN],
                       uint8_t sig_s[ESP_SM2_PARAM_LEN]);

/**
 * @brief Verify an SM2 signature (GB/T 32918.2) over a digest.
 *
 * The caller computes e = SM3(ZA || M). All buffers are big-endian and
 * ESP_SM2_PARAM_LEN bytes long. The public key is validated on-curve in
 * hardware before use.
 *
 * @param digest Digest e that was signed.
 * @param pub_x  Public key X coordinate.
 * @param pub_y  Public key Y coordinate.
 * @param sig_r  Signature component r. Must satisfy 1 <= r <= n-1.
 * @param sig_s  Signature component s. Must satisfy 1 <= s <= n-1.
 *
 * @return ESP_OK when the signature is valid. ESP_ERR_INVALID_ARG on a
 *         NULL pointer or an out-of-range r or s. ESP_ERR_NOT_SUPPORTED
 *         when the DIS_SM_CRYPT eFuse disables SM crypto. ESP_FAIL when
 *         the signature or the public key is invalid.
 */
esp_err_t esp_sm2_verify(const uint8_t digest[ESP_SM2_PARAM_LEN],
                         const uint8_t pub_x[ESP_SM2_PARAM_LEN],
                         const uint8_t pub_y[ESP_SM2_PARAM_LEN],
                         const uint8_t sig_r[ESP_SM2_PARAM_LEN],
                         const uint8_t sig_s[ESP_SM2_PARAM_LEN]);

#ifdef __cplusplus
}
#endif

#endif /* SOC_ECC_SUPPORT_CURVE_SM2 && SOC_ECC_EXTENDED_MODES_SUPPORTED */
