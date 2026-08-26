/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "mbedtls/platform_util.h"
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
    if (alg != PSA_ALG_MD5) {
        return PSA_ALG_IS_HASH(alg) ? PSA_ERROR_NOT_SUPPORTED
                                    : PSA_ERROR_INVALID_ARGUMENT;
    }
    if (hash_size < PSA_HASH_LENGTH(PSA_ALG_MD5)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    md5_context_t ctx = {0};
    esp_rom_md5_init(&ctx);
    esp_rom_md5_update(&ctx, input, input_length);
    esp_rom_md5_final(hash, &ctx);
    *hash_length = PSA_HASH_LENGTH(PSA_ALG_MD5);
    mbedtls_platform_zeroize(&ctx, sizeof(ctx));
    return PSA_SUCCESS;
}

psa_status_t esp_md5_hash_setup(esp_md5_hash_operation_t *operation,
                                psa_algorithm_t alg)
{
    if (operation == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (alg != PSA_ALG_MD5) {
        return PSA_ALG_IS_HASH(alg) ? PSA_ERROR_NOT_SUPPORTED
                                    : PSA_ERROR_INVALID_ARGUMENT;
    }
    if (operation->is_active) {
        return PSA_ERROR_BAD_STATE;
    }
    esp_rom_md5_init(&operation->md5_ctx);
    operation->is_active = true;
    return PSA_SUCCESS;
}

psa_status_t esp_md5_hash_update(
    esp_md5_hash_operation_t *operation,
    const uint8_t *input,
    size_t input_length )
{
    if (operation == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (input == NULL && input_length != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (!operation->is_active) {
        return PSA_ERROR_BAD_STATE;
    }
    if (input_length == 0) {
        return PSA_SUCCESS;
    }
    esp_rom_md5_update(&operation->md5_ctx, input, input_length);
    return PSA_SUCCESS;
}

psa_status_t esp_md5_hash_finish(
    esp_md5_hash_operation_t *operation,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length)
{
    if (operation == NULL || hash == NULL || hash_length == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (!operation->is_active) {
        return PSA_ERROR_BAD_STATE;
    }
    if (hash_size < PSA_HASH_LENGTH(PSA_ALG_MD5)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    esp_rom_md5_final(hash, &operation->md5_ctx);
    *hash_length = PSA_HASH_LENGTH(PSA_ALG_MD5);
    mbedtls_platform_zeroize(operation, sizeof(*operation));
    return PSA_SUCCESS;
}

psa_status_t esp_md5_hash_abort(esp_md5_hash_operation_t *operation)
{
    if (operation == NULL) {
        return PSA_SUCCESS;
    }
    mbedtls_platform_zeroize(operation, sizeof(*operation));
    return PSA_SUCCESS;
}

psa_status_t esp_md5_hash_clone(
    const esp_md5_hash_operation_t *source_operation,
    esp_md5_hash_operation_t *target_operation)
{
    if (source_operation == NULL || target_operation == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (!source_operation->is_active) {
        return PSA_ERROR_BAD_STATE;
    }
    if (target_operation->is_active) {
        return PSA_ERROR_BAD_STATE;
    }
    memcpy(target_operation, source_operation, sizeof(esp_md5_hash_operation_t));
    return PSA_SUCCESS;
}
