/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mbedtls/md.h"

#define FACTOR_KEYLEN_IN_BYTES 4
#define SWAP_INT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))

/**
 * @brief Get the data context for the digital signature.
 *
 * @return esp_ds_data_t*   esp ds data context pointer.
 */
esp_ds_data_t *esp_ds_get_data_ctx(void);

/**
 * @brief Get the HMAC key ID used for digital signature operations.
 *
 * @return hmac_key_id_t    HMAC key ID.
 */
hmac_key_id_t esp_ds_get_hmac_key_id(void);

/**
 * @brief Mask generation function (MGF) for TLS 1.3.
 *
 * @param[in] dst Pointer to the destination buffer.
 * @param[in] dlen Length of the destination buffer.
 * @param[in] src Pointer to the source buffer.
 * @param[in] slen Length of the salt value.
 * @param[in] md_alg The message digest algorithm type.
 * @return int Returns 0 on success, or a negative error code on failure.
 */
int esp_ds_mgf_mask(unsigned char *dst, size_t dlen, unsigned char *src,
	         size_t slen, mbedtls_md_type_t md_alg);

/**
 * @brief Generates a hash using the M-Prime algorithm as specified in RFC 8446.
 *
 * @param hash  Pointer to the input hash.
 * @param hlen  Length of the input hash.
 * @param salt  Pointer to the salt value.
 * @param slen  Length of the salt value.
 * @param out   Pointer to the output buffer where the hash will be stored.
 * @param md_alg    The message digest algorithm type to use for hashing.
 * @return int Returns 0 on success, or a negative error code on failure.
 */
int esp_ds_hash_mprime(const unsigned char *hash, size_t hlen,
    const unsigned char *salt, size_t slen,
    unsigned char *out, mbedtls_md_type_t md_alg);

#ifdef __cplusplus
}
#endif
