/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#if defined(ESP_SHA_DRIVER_ENABLED)
#ifndef PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#define PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#endif

#include <stdbool.h>
#include "psa_crypto_driver_esp_sha_contexts.h"
#include "psa/crypto.h"

// /* Include function declarations from individual SHA modules */
// #ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
// #include "../esp_sha/include/psa_crypto_driver_esp_sha1.h"
// #endif /* MBEDTLS_PSA_ACCEL_ALG_SHA_1 */

// #ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_256
// #include "../esp_sha/include/psa_crypto_driver_esp_sha256.h"
// #endif

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
#endif
