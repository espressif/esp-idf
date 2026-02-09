/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once


#include "esp_types.h"
#include "psa/crypto.h"
#include "esp_rom_md5.h"

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

psa_status_t esp_md5_hash_setup(md5_context_t *operation,
                                psa_algorithm_t alg);

psa_status_t esp_md5_hash_update(
    md5_context_t *operation,
    const uint8_t *input,
    size_t input_length );

psa_status_t esp_md5_hash_finish(
    md5_context_t *operation,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length);

psa_status_t esp_md5_hash_abort(md5_context_t *operation);

psa_status_t esp_md5_hash_clone(
    const md5_context_t *source_operation,
    md5_context_t *target_operation);

#ifdef __cplusplus
}
#endif
