/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "psa_crypto_driver_esp_md5.h"

psa_status_t esp_md5_hash_compute(psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length)
{
    if (hash == NULL || hash_length == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (input_length > 0 && input == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (hash_size < PSA_HASH_LENGTH(PSA_ALG_MD5)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    if (alg != PSA_ALG_MD5) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    md5_context_t operation = {0};
    esp_rom_md5_init(&operation);
    esp_rom_md5_update(&operation, input, input_length);
    esp_rom_md5_final(hash, &operation);
    *hash_length = PSA_HASH_LENGTH(PSA_ALG_MD5);
    return PSA_SUCCESS;
}

psa_status_t esp_md5_hash_setup(md5_context_t *operation,
                                psa_algorithm_t alg)
{
    if (operation == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (alg != PSA_ALG_MD5) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    esp_rom_md5_init(operation);
    return PSA_SUCCESS;
}

psa_status_t esp_md5_hash_update(
    md5_context_t *operation,
    const uint8_t *input,
    size_t input_length )
{
    if (operation == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (input_length > 0 && input == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    esp_rom_md5_update(operation, input, input_length);
    return PSA_SUCCESS;
}

psa_status_t esp_md5_hash_finish(
    md5_context_t *operation,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length)
{
    if (operation == NULL || hash == NULL || hash_length == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (hash_size < PSA_HASH_LENGTH(PSA_ALG_MD5)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    esp_rom_md5_final(hash, operation);
    *hash_length = PSA_HASH_LENGTH(PSA_ALG_MD5);
    return PSA_SUCCESS;
}

psa_status_t esp_md5_hash_abort(md5_context_t *operation)
{
    if (operation == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    memset(operation, 0, sizeof(*operation));
    return PSA_SUCCESS;
}

psa_status_t esp_md5_hash_clone(
    const md5_context_t *source_operation,
    md5_context_t *target_operation)
{
    if (source_operation == NULL || target_operation == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    memcpy(target_operation, source_operation, sizeof(md5_context_t));
    return PSA_SUCCESS;
}
