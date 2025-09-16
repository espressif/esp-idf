/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ESP_AES_DRIVER_ENABLED) || defined(PSA_CRYPTO_DRIVER_TEST)

#if defined(ESP_SHA_DRIVER_ENABLED)
#include "psa_crypto_driver_esp_sha_contexts.h"
#include "psa_crypto_driver_esp_sha.h"
#endif /* ESP_SHA_DRIVER_ENABLED */

#define PSA_AES_BLOCK_SIZE PSA_BLOCK_CIPHER_BLOCK_LENGTH(PSA_KEY_TYPE_AES)
#define PSA_DES_BLOCK_SIZE PSA_BLOCK_CIPHER_BLOCK_LENGTH(PSA_KEY_TYPE_DES)

#if defined(PSA_WANT_KEY_TYPE_AES)
#define PSA_CMAC_MAX_BLOCK_SIZE PSA_AES_BLOCK_SIZE /**< The longest block used by CMAC is that of AES. */
#else
#define PSA_CMAC_MAX_BLOCK_SIZE PSA_DES_BLOCK_SIZE /**< The longest block used by CMAC is that of 3DES. */
#endif

typedef struct {
    /** The CMAC key identifier for cipher operations */
    psa_key_id_t key_id;

    /** The internal state of the CMAC algorithm.  */
    unsigned char state[PSA_CMAC_MAX_BLOCK_SIZE];

    /** Unprocessed data - either data that was not block aligned and is still
     *  pending processing, or the final block. */
    unsigned char unprocessed_block[PSA_CMAC_MAX_BLOCK_SIZE];

    /** The length of data pending processing. */
    size_t unprocessed_len;

    uint8_t cipher_block_length;

    struct psa_cipher_operation_s cipher_ctx;

    psa_algorithm_t alg;
#if defined(ESP_SHA_DRIVER_ENABLED)
    esp_sha_hash_operation_t hmac_operation;
    uint8_t opad[PSA_HMAC_MAX_HASH_BLOCK_SIZE];
#endif /* ESP_SHA_DRIVER_ENABLED */
} esp_cmac_operation_t;

#endif /* ESP_AES_DRIVER_ENABLED */

#ifdef __cplusplus
}
#endif
