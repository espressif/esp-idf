/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "mbedtls/esp_config.h"
#include "psa_crypto_driver_esp_sha.h"
#include "include/psa_crypto_driver_esp_sha1.h"
#include "include/psa_crypto_driver_esp_sha256.h"
#include "include/psa_crypto_driver_esp_sha512.h"
#include "psa/crypto.h"
#include "psa/crypto_sizes.h"
#include "mbedtls/platform_util.h"
#include "esp_log.h"
#include "esp_heap_caps.h"

#if CONFIG_SOC_SHA_GDMA
#include "esp_sha_internal.h"
#endif
#include "sha/sha_core.h"

#include "esp_err.h"

static int esp_sha_driver_check_supported_algorithm(psa_algorithm_t alg) {
    if (0
        #ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
        || alg == PSA_ALG_SHA_1
        #endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1
        #ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_224
        || alg == PSA_ALG_SHA_224
        #endif // MBEDTLS_PSA_ACCEL_ALG_SHA_224
        #ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_256
        || alg == PSA_ALG_SHA_256
        #endif // MBEDTLS_PSA_ACCEL_ALG_SHA_256
        #ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_384
        || alg == PSA_ALG_SHA_384
        #endif // MBEDTLS_PSA_ACCEL_ALG_SHA_384
        #ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_512
        || alg == PSA_ALG_SHA_512
        #endif // MBEDTLS_PSA_ACCEL_ALG_SHA_512
    ) {
        return ESP_OK;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

static int esp_sha_validate_args(psa_algorithm_t alg, const uint8_t *input, size_t input_length, uint8_t *hash, size_t hash_size, const size_t *hash_length) {
    if (!hash || !hash_length) {
        return ESP_ERR_INVALID_ARG;
    }
    if (input == NULL && input_length != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t expected_hash_size = PSA_HASH_LENGTH(alg);
    if (hash_size < expected_hash_size) {
        return ESP_ERR_INVALID_SIZE;
    }

    return ESP_OK;
}

psa_status_t esp_sha_hash_compute(
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length)
{
    int ret = esp_sha_driver_check_supported_algorithm(alg);
    if (ret != ESP_OK) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    ret = esp_sha_validate_args(alg, input, input_length, hash, hash_size, hash_length);
    if (ret == ESP_ERR_INVALID_ARG) {
        return PSA_ERROR_INVALID_ARGUMENT;
    } else if (ret == ESP_ERR_INVALID_SIZE) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    } else if (ret != ESP_OK) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    size_t hash_length_calculated = 0;
    switch(alg) {
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
        case PSA_ALG_SHA_1:
            esp_sha1_context sha1_ctx = {0};
            ret = esp_sha1_driver_compute(&sha1_ctx, input, input_length, hash, hash_size, &hash_length_calculated);
            mbedtls_platform_zeroize(&sha1_ctx, sizeof(sha1_ctx));
            *hash_length = hash_length_calculated;
            break;
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_224
        case PSA_ALG_SHA_224:
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_224
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_256
        case PSA_ALG_SHA_256:
            esp_sha256_context sha256_ctx = {0};
            ret = esp_sha256_driver_compute(&sha256_ctx, alg, input, input_length, hash, hash_size, &hash_length_calculated);
            mbedtls_platform_zeroize(&sha256_ctx, sizeof(sha256_ctx));
            *hash_length = hash_length_calculated;
            break;
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_256
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_384
        case PSA_ALG_SHA_384:
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_384
#if defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384)
        case PSA_ALG_SHA_512:
            esp_sha512_context sha512_ctx = {0};
            ret = esp_sha512_driver_compute(&sha512_ctx, alg, input, input_length, hash, hash_size, &hash_length_calculated);
            mbedtls_platform_zeroize(&sha512_ctx, sizeof(sha512_ctx));
            *hash_length = hash_length_calculated;
            break;
#endif // defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384)
        default:
            return PSA_ERROR_NOT_SUPPORTED;
    }

    if (ret != ESP_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }
    return PSA_SUCCESS;
}

psa_status_t esp_sha_hash_setup(esp_sha_hash_operation_t *operation, psa_algorithm_t alg)
{
    if (!operation) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (operation->sha_ctx != NULL) {
        return PSA_ERROR_BAD_STATE;
    }
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
    if (alg == PSA_ALG_SHA_1) {
        esp_sha1_context *sha1_ctx = heap_caps_malloc(sizeof(esp_sha1_context), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        if (!sha1_ctx) {
            return PSA_ERROR_INSUFFICIENT_MEMORY;
        }
        memset(sha1_ctx, 0, sizeof(esp_sha1_context));
        operation->sha_ctx = sha1_ctx;
        operation->sha_type = ESP_SHA_OPERATION_TYPE_SHA1;
        int ret = esp_sha1_starts(sha1_ctx);
        if (ret != ESP_OK) {
            esp_sha_hash_abort(operation);
            return PSA_ERROR_HARDWARE_FAILURE;
        }
        return PSA_SUCCESS;
    } else
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1
#if defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_224)
    if (
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_224
        alg == PSA_ALG_SHA_224 ||
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_224
        alg == PSA_ALG_SHA_256) {
        esp_sha256_context *sha256_ctx = heap_caps_malloc(sizeof(esp_sha256_context), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        if (!sha256_ctx) {
            return PSA_ERROR_INSUFFICIENT_MEMORY;
        }
        memset(sha256_ctx, 0, sizeof(esp_sha256_context));
        int mode;
        operation->sha_ctx = sha256_ctx;
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_224
        operation->sha_type = (alg == PSA_ALG_SHA_224) ? ESP_SHA_OPERATION_TYPE_SHA224 : ESP_SHA_OPERATION_TYPE_SHA256;
        mode = (alg == PSA_ALG_SHA_224) ? SHA2_224 : SHA2_256;
#else
        operation->sha_type = ESP_SHA_OPERATION_TYPE_SHA256;
        mode = SHA2_256;
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_224
        int ret = esp_sha256_starts(sha256_ctx, mode);
        if (ret != PSA_SUCCESS) {
            esp_sha_hash_abort(operation);
            return ret;
        }
        return PSA_SUCCESS;
    } else
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_256 || MBEDTLS_PSA_ACCEL_ALG_SHA_224
#if defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384)
    if (
#if defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384)
        alg == PSA_ALG_SHA_384 ||
#endif // defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384)
        alg == PSA_ALG_SHA_512) {
        esp_sha512_context *sha512_ctx = heap_caps_malloc(sizeof(esp_sha512_context), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        if (!sha512_ctx) {
            return PSA_ERROR_INSUFFICIENT_MEMORY;
        }
        memset(sha512_ctx, 0, sizeof(esp_sha512_context));
        int mode = SHA2_512;
        operation->sha_ctx = sha512_ctx;
        operation->sha_type = ESP_SHA_OPERATION_TYPE_SHA512;
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_384
        operation->sha_type = (alg == PSA_ALG_SHA_384) ? ESP_SHA_OPERATION_TYPE_SHA384 : ESP_SHA_OPERATION_TYPE_SHA512;
        mode = (alg == PSA_ALG_SHA_384) ? SHA2_384 : SHA2_512;
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_384
        int ret = esp_sha512_starts(sha512_ctx, mode);
        if (ret != PSA_SUCCESS) {
            esp_sha_hash_abort(operation);
            return ret;
        }
        return PSA_SUCCESS;
    }
#endif // defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384)
    if (PSA_ALG_IS_HASH(alg)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    return PSA_ERROR_INVALID_ARGUMENT;
}

psa_status_t esp_sha_hash_update(
    esp_sha_hash_operation_t *operation,
    const uint8_t *input,
    size_t input_length)
{
    if (!operation) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (operation->sha_ctx == NULL) {
        return PSA_ERROR_BAD_STATE;
    }
    if (input == NULL && input_length != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (input_length == 0) {
        return PSA_SUCCESS;
    }
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
    if (operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA1) {
        esp_sha1_context *ctx = (esp_sha1_context *)operation->sha_ctx;
        return esp_sha1_driver_update(ctx, input, input_length);
    } else
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1
#if defined(MBEDTLS_PSA_ACCEL_ALG_SHA_224) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256)
    if (operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA256 || operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA224) {
        esp_sha256_context *ctx = (esp_sha256_context *)operation->sha_ctx;
        return esp_sha256_driver_update(ctx, input, input_length);
    } else
#endif // defined(MBEDTLS_PSA_ACCEL_ALG_SHA_224) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256)
#if (defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512))
    if (operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA384 || operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA512) {
        esp_sha512_context *ctx = (esp_sha512_context *)operation->sha_ctx;
        return esp_sha512_driver_update(ctx, input, input_length);
    }
#endif // (defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512))
    return PSA_ERROR_BAD_STATE;
}

psa_status_t esp_sha_hash_finish(
    esp_sha_hash_operation_t *operation,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length)
{
    if (!operation || !hash || !hash_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (operation->sha_ctx == NULL) {
        return PSA_ERROR_BAD_STATE;
    }

    /* Defense in depth: gate buffer sizing in the dispatcher so we don't rely
     * solely on every per-algorithm sub-driver to enforce it. */
    size_t expected_size = 0;
    switch (operation->sha_type) {
        case ESP_SHA_OPERATION_TYPE_SHA1:   expected_size = PSA_HASH_LENGTH(PSA_ALG_SHA_1); break;
        case ESP_SHA_OPERATION_TYPE_SHA224: expected_size = PSA_HASH_LENGTH(PSA_ALG_SHA_224); break;
        case ESP_SHA_OPERATION_TYPE_SHA256: expected_size = PSA_HASH_LENGTH(PSA_ALG_SHA_256); break;
        case ESP_SHA_OPERATION_TYPE_SHA384: expected_size = PSA_HASH_LENGTH(PSA_ALG_SHA_384); break;
        case ESP_SHA_OPERATION_TYPE_SHA512: expected_size = PSA_HASH_LENGTH(PSA_ALG_SHA_512); break;
        default: return PSA_ERROR_BAD_STATE;
    }
    if (hash_size < expected_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
    if (operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA1) {
        return esp_sha1_driver_finish((esp_sha1_context *)operation->sha_ctx,
                                      hash, hash_size, hash_length);
    } else
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1
#if defined(MBEDTLS_PSA_ACCEL_ALG_SHA_224) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256)
    if (operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA256 ||
        operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA224) {
        return esp_sha256_driver_finish((esp_sha256_context *)operation->sha_ctx,
                                        hash, hash_size, hash_length,
                                        operation->sha_type);
    } else
#endif // defined(MBEDTLS_PSA_ACCEL_ALG_SHA_224) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256)
#if (defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512))
    if (operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA384 ||
        operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA512) {
        return esp_sha512_driver_finish((esp_sha512_context *)operation->sha_ctx,
                                        hash, hash_size, hash_length,
                                        operation->sha_type);
    }
#endif // defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512)

    return PSA_ERROR_BAD_STATE;
}

psa_status_t esp_sha_hash_abort(esp_sha_hash_operation_t *operation)
{
    if (!operation) {
        return PSA_SUCCESS;
    }

    if (operation->sha_ctx == NULL) {
        return PSA_SUCCESS;
    }

    bool bad_state = false;
    switch (operation->sha_type) {
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
        case ESP_SHA_OPERATION_TYPE_SHA1:
            (void)esp_sha1_driver_abort((esp_sha1_context *)operation->sha_ctx);
            break;
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1
#if defined(MBEDTLS_PSA_ACCEL_ALG_SHA_224) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256)
        case ESP_SHA_OPERATION_TYPE_SHA256:
        case ESP_SHA_OPERATION_TYPE_SHA224:
            (void)esp_sha256_driver_abort((esp_sha256_context *)operation->sha_ctx);
            break;
#endif // defined(MBEDTLS_PSA_ACCEL_ALG_SHA_224) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256)
#if (defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512))
        case ESP_SHA_OPERATION_TYPE_SHA384:
        case ESP_SHA_OPERATION_TYPE_SHA512:
            (void)esp_sha512_driver_abort((esp_sha512_context *)operation->sha_ctx);
            break;
#endif // (defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512))
        default:
            bad_state = true;
            break;
    }

    free(operation->sha_ctx);
    operation->sha_ctx = NULL;
    return bad_state ? PSA_ERROR_BAD_STATE : PSA_SUCCESS;
}

psa_status_t esp_sha_hash_clone(
    const esp_sha_hash_operation_t *source_operation,
    esp_sha_hash_operation_t *target_operation)
{
    if (!source_operation || !target_operation) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (source_operation->sha_ctx == NULL) {
        return PSA_ERROR_BAD_STATE;
    }
    if (target_operation->sha_ctx != NULL) {
        return PSA_ERROR_BAD_STATE;
    }

    void *ctx = NULL;
    size_t ctx_size = 0;
    psa_status_t status = PSA_ERROR_NOT_SUPPORTED;

#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
    if (source_operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA1) {
        ctx_size = sizeof(esp_sha1_context);
    } else
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1
#if defined(MBEDTLS_PSA_ACCEL_ALG_SHA_224) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256)
    if (source_operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA256 ||
        source_operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA224) {
        ctx_size = sizeof(esp_sha256_context);
    } else
#endif // defined(MBEDTLS_PSA_ACCEL_ALG_SHA_224) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256)
#if (defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512))
    if (source_operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA384 ||
        source_operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA512) {
        ctx_size = sizeof(esp_sha512_context);
    } else
#endif // (defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512))
    {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    ctx = heap_caps_malloc(ctx_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (!ctx) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }
    memset(ctx, 0, ctx_size);

#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
    if (source_operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA1) {
        status = esp_sha1_driver_clone(source_operation->sha_ctx, ctx);
    } else
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1
#if defined(MBEDTLS_PSA_ACCEL_ALG_SHA_224) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256)
    if (source_operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA256 ||
        source_operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA224) {
        status = esp_sha256_driver_clone(source_operation->sha_ctx, ctx);
    } else
#endif // defined(MBEDTLS_PSA_ACCEL_ALG_SHA_224) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256)
#if (defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512))
    if (source_operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA384 ||
        source_operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA512) {
        status = esp_sha512_driver_clone(source_operation->sha_ctx, ctx);
    } else
#endif // (defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512))
    {
        status = PSA_ERROR_NOT_SUPPORTED;
    }

    if (status != PSA_SUCCESS) {
        mbedtls_platform_zeroize(ctx, ctx_size);
        free(ctx);
        return status;
    }

    target_operation->sha_ctx = ctx;
    target_operation->sha_type = source_operation->sha_type;
    return PSA_SUCCESS;
}
