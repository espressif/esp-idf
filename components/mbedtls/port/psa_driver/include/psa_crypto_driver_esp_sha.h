/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "psa/crypto.h"
#include "psa_crypto_driver_esp_sha_contexts.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_MBEDTLS_HARDWARE_SHA
#ifndef PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#define PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#endif
#endif // CONFIG_MBEDTLS_HARDWARE_SHA

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ((n) >> 24);       \
    (b)[(i) + 1] = (unsigned char) ((n) >> 16);       \
    (b)[(i) + 2] = (unsigned char) ((n) >>  8);       \
    (b)[(i) + 3] = (unsigned char) ((n)      );       \
}
#endif

psa_status_t esp_sha_hash_compute(
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length);

psa_status_t esp_sha_hash_setup(esp_sha_hash_operation_t *operation,
                                psa_algorithm_t alg);

psa_status_t esp_sha_hash_update(
    esp_sha_hash_operation_t *operation,
    const uint8_t *input,
    size_t input_length );

psa_status_t esp_sha_hash_finish(
    esp_sha_hash_operation_t *operation,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length);

psa_status_t esp_sha_hash_abort(esp_sha_hash_operation_t *operation);

psa_status_t esp_sha_hash_clone(
    const esp_sha_hash_operation_t *source_operation,
    esp_sha_hash_operation_t *target_operation);

int esp_internal_sha1_process( esp_sha1_context *ctx, const unsigned char data[64] );
int esp_internal_sha256_process( esp_sha256_context *ctx, const unsigned char data[64] );
int esp_internal_sha512_process( esp_sha512_context *ctx, const unsigned char data[128] );
int esp_sha1_starts(esp_sha1_context *ctx);
int esp_sha1_update(esp_sha1_context *ctx, const unsigned char *input, size_t ilen);
int esp_sha1_finish(esp_sha1_context *ctx, uint8_t *output);
#ifdef __cplusplus
}
#endif
