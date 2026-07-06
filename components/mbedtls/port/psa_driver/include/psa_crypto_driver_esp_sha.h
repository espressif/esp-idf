/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "psa/crypto.h"
#include "psa_crypto_driver_esp_sha_contexts.h"
#include "soc/soc_caps.h"

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

/**
 * @brief Direct (non-PSA) entry points to the parallel-engine SHA-256 driver.
 *
 * @note These declarations are intentionally restricted to targets that use
 *       the parallel-engine peripheral (currently only ESP32). On targets
 *       using the "core" SHA peripheral the equivalent functions are kept
 *       static inside the driver, since the PSA dispatcher and ROM SHA
 *       cover all in-tree consumers.
 *
 * @note These bypass PSA. They exist for special callers — most notably
 *       espcoredump — that must run from a panic context where the PSA
 *       code path's malloc / FreeRTOS-locked HW engine is unsafe. Such
 *       callers are expected to set @c operation_mode to
 *       @c ESP_SHA_MODE_SOFTWARE on the context immediately after
 *       @c esp_sha256_starts so that subsequent calls never enter the
 *       hardware path. Regular consumers should keep using the PSA API.
 */
#if CONFIG_SOC_SHA_SUPPORT_PARALLEL_ENG || __DOXYGEN__
/**
 * @brief Initialise an esp_sha256_context for a new SHA-224/256 digest.
 *
 * @param ctx   Context to initialise. Must be non-NULL.
 * @param mode  Either @c SHA2_224 or @c SHA2_256 (from @c hal/sha_types.h).
 *
 * @return @c PSA_SUCCESS on success.
 */
psa_status_t esp_sha256_starts(esp_sha256_context *ctx, int mode);

/**
 * @brief Feed input bytes into a SHA-224/256 digest in progress.
 *
 * @param ctx    Previously started context.
 * @param input  Input buffer; may be NULL only if @p ilen is 0.
 * @param ilen   Number of bytes to absorb.
 *
 * @return 0 on success, negative on internal error.
 */
int esp_sha256_update(esp_sha256_context *ctx, const unsigned char *input, size_t ilen);

/**
 * @brief Finalise the digest and write the output.
 *
 * @param ctx     Context that has been started and (optionally) updated.
 * @param output  Buffer for the final digest. Must be at least 28 bytes
 *                for SHA-224 or 32 bytes for SHA-256.
 *
 * @return 0 on success, negative on internal error.
 */
int esp_sha256_finish(esp_sha256_context *ctx, unsigned char *output);
#endif /* CONFIG_SOC_SHA_SUPPORT_PARALLEL_ENG */
#ifdef __cplusplus
}
#endif
