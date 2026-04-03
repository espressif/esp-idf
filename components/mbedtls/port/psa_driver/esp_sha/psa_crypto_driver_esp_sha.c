/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "mbedtls/esp_config.h"
#include "psa_crypto_driver_esp_sha.h"
#include "include/psa_crypto_driver_esp_sha1.h"
#include "include/psa_crypto_driver_esp_sha256.h"
#include "include/psa_crypto_driver_esp_sha512.h"
#include "psa/crypto.h"
#include "psa/crypto_sizes.h"
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

static int esp_sha_validate_args(psa_algorithm_t alg, const uint8_t *input, size_t input_length, uint8_t *hash, size_t hash_size) {
    if (!hash) {
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

    ret = esp_sha_validate_args(alg, input, input_length, hash, hash_size);
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
            memset(&sha1_ctx, 0, sizeof(sha1_ctx));
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
            memset(&sha256_ctx, 0, sizeof(sha256_ctx));
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
            memset(&sha512_ctx, 0, sizeof(sha512_ctx));
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
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
    if (alg == PSA_ALG_SHA_1) {
        esp_sha1_context *sha1_ctx = heap_caps_malloc(sizeof(esp_sha1_context), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        if (!sha1_ctx) {
            return PSA_ERROR_INSUFFICIENT_MEMORY;
        }
        memset(sha1_ctx, 0, sizeof(esp_sha1_context));
        operation->sha_ctx = sha1_ctx;
        operation->sha_type = ESP_SHA_OPERATION_TYPE_SHA1;
        return esp_sha1_starts(sha1_ctx);
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
        operation->sha_ctx = sha256_ctx;
        int mode;
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_224
        operation->sha_type = (alg == PSA_ALG_SHA_224) ? ESP_SHA_OPERATION_TYPE_SHA224 : ESP_SHA_OPERATION_TYPE_SHA256;
        mode = (alg == PSA_ALG_SHA_224) ? SHA2_224 : SHA2_256;
#else
        operation->sha_type = ESP_SHA_OPERATION_TYPE_SHA256;
        mode = SHA2_256;
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_224
        return esp_sha256_starts(sha256_ctx, mode);
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
        operation->sha_ctx = sha512_ctx;
        int mode = SHA2_512;
        operation->sha_type = ESP_SHA_OPERATION_TYPE_SHA512;
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_384
        operation->sha_type = (alg == PSA_ALG_SHA_384) ? ESP_SHA_OPERATION_TYPE_SHA384 : ESP_SHA_OPERATION_TYPE_SHA512;
        mode = (alg == PSA_ALG_SHA_384) ? SHA2_384 : SHA2_512;
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_384
        return esp_sha512_starts(sha512_ctx, mode);
    }
#endif // defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384)
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t esp_sha_hash_update(
    esp_sha_hash_operation_t *operation,
    const uint8_t *input,
    size_t input_length)
{
    if (!operation || !operation->sha_ctx || !input) {
        return PSA_ERROR_INVALID_ARGUMENT;
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
    return PSA_ERROR_NOT_SUPPORTED;
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

#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
    if (operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA1) {
        esp_sha1_context *ctx = (esp_sha1_context *)operation->sha_ctx;
        int ret = esp_sha1_driver_finish(ctx, hash, hash_size, hash_length);
        free(ctx); // Free the context after use
        operation->sha_ctx = NULL;
        return ret;
    } else
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1
#if defined(MBEDTLS_PSA_ACCEL_ALG_SHA_224) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256)
    if (operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA256 ||
        operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA224) {
        esp_sha256_context *ctx = (esp_sha256_context *)operation->sha_ctx;
        int ret = esp_sha256_driver_finish(ctx, hash, hash_size, hash_length, operation->sha_type);
        free(ctx); // Free the context after use
        operation->sha_ctx = NULL;
        return ret;
    } else
#endif // defined(MBEDTLS_PSA_ACCEL_ALG_SHA_224) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256)
#if (defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512))
    if (operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA384 ||
        operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA512) {
        esp_sha512_context *ctx = (esp_sha512_context *)operation->sha_ctx;
        int ret = esp_sha512_driver_finish(ctx, hash, hash_size, hash_length, operation->sha_type);
        free(ctx); // Free the context after use
        operation->sha_ctx = NULL;
        return ret;
    }
#endif // defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512)

    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t esp_sha_hash_abort(esp_sha_hash_operation_t *operation)
{
    if (!operation) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
    if (operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA1) {
        esp_sha1_context *ctx = (esp_sha1_context *)operation->sha_ctx;
        if (ctx) {
            esp_sha1_driver_abort(ctx);
        }
    } else
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1
#if defined(MBEDTLS_PSA_ACCEL_ALG_SHA_224) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256)
    if (operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA256 ||
        operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA224) {
        esp_sha256_context *ctx = (esp_sha256_context *)operation->sha_ctx;
        if (ctx) {
            esp_sha256_driver_abort(ctx);
        }
    } else
#endif // defined(MBEDTLS_PSA_ACCEL_ALG_SHA_224) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256)
#if (defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512))
    if (operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA384 ||
        operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA512) {
        esp_sha512_context *ctx = (esp_sha512_context *)operation->sha_ctx;
        if (ctx) {
            esp_sha512_driver_abort(ctx);
        }
    } else
#endif // (defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512))
    {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    if (operation->sha_ctx) {
        free(operation->sha_ctx);
        operation->sha_ctx = NULL;
    }

    return PSA_SUCCESS;
}

psa_status_t esp_sha_hash_clone(
    const esp_sha_hash_operation_t *source_operation,
    esp_sha_hash_operation_t *target_operation)
{
    target_operation->sha_type = source_operation->sha_type;
#ifdef MBEDTLS_PSA_ACCEL_ALG_SHA_1
    if (target_operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA1) {
        esp_sha1_context *sha1_ctx = heap_caps_malloc(sizeof(esp_sha1_context), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        if (!sha1_ctx) {
            return PSA_ERROR_INSUFFICIENT_MEMORY;
        }
        memset(sha1_ctx, 0, sizeof(esp_sha1_context));
        target_operation->sha_ctx = sha1_ctx;
        if (!target_operation->sha_ctx) {
            return PSA_ERROR_INSUFFICIENT_MEMORY;
        }
        esp_sha1_driver_clone(source_operation->sha_ctx, target_operation->sha_ctx);
    } else
#endif // MBEDTLS_PSA_ACCEL_ALG_SHA_1
#if defined(MBEDTLS_PSA_ACCEL_ALG_SHA_224) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256)
    if (target_operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA256 ||
               target_operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA224) {
        esp_sha256_context *sha256_ctx = heap_caps_malloc(sizeof(esp_sha256_context), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        if (!sha256_ctx) {
            return PSA_ERROR_INSUFFICIENT_MEMORY;
        }
        memset(sha256_ctx, 0, sizeof(esp_sha256_context));
        target_operation->sha_ctx = sha256_ctx;
        if (!target_operation->sha_ctx) {
            return PSA_ERROR_INSUFFICIENT_MEMORY;
        }
        esp_sha256_driver_clone(source_operation->sha_ctx, target_operation->sha_ctx);
    } else
#endif // defined(MBEDTLS_PSA_ACCEL_ALG_SHA_224) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256)
#if (defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512))
    if (target_operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA384 ||
               target_operation->sha_type == ESP_SHA_OPERATION_TYPE_SHA512) {
        esp_sha512_context *sha512_ctx = heap_caps_malloc(sizeof(esp_sha512_context), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        if (!sha512_ctx) {
            return PSA_ERROR_INSUFFICIENT_MEMORY;
        }
        memset(sha512_ctx, 0, sizeof(esp_sha512_context));
        target_operation->sha_ctx = sha512_ctx;
        if (!target_operation->sha_ctx) {
            return PSA_ERROR_INSUFFICIENT_MEMORY;
        }
        esp_sha512_driver_clone(source_operation->sha_ctx, target_operation->sha_ctx);
    } else
#endif // (defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384) || defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512))
    {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    return PSA_SUCCESS;
}
