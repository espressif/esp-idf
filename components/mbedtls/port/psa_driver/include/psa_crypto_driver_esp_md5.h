/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once


#include "esp_types.h"
#include "psa/crypto.h"
#include "psa_crypto_driver_esp_md5_contexts.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_MBEDTLS_ROM_MD5
#ifndef PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#define PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#endif
#endif // CONFIG_MBEDTLS_ROM_MD5

psa_status_t esp_md5_hash_compute(psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length);

psa_status_t esp_md5_hash_setup(esp_md5_hash_operation_t *operation,
                                psa_algorithm_t alg);

psa_status_t esp_md5_hash_update(
    esp_md5_hash_operation_t *operation,
    const uint8_t *input,
    size_t input_length );

psa_status_t esp_md5_hash_finish(
    esp_md5_hash_operation_t *operation,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length);

psa_status_t esp_md5_hash_abort(esp_md5_hash_operation_t *operation);

psa_status_t esp_md5_hash_clone(
    const esp_md5_hash_operation_t *source_operation,
    esp_md5_hash_operation_t *target_operation);

#ifdef __cplusplus
}
#endif
