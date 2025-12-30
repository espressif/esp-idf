/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#pragma once

#if defined(ESP_SHA_DRIVER_ENABLED)

#include <stdint.h>
#include <stddef.h>

/* Forward declarations to avoid circular dependencies */
typedef uint32_t psa_algorithm_t;
typedef int32_t psa_status_t;

psa_status_t esp_sha512_driver_compute(
    esp_sha512_context *ctx,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length);

psa_status_t esp_sha512_starts(esp_sha512_context *ctx, int mode);

psa_status_t esp_sha512_driver_update(
    esp_sha512_context *ctx,
    const uint8_t *input,
    size_t input_length);

psa_status_t esp_sha512_driver_finish(
    esp_sha512_context *ctx,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length,
    esp_sha_operation_type_t sha_type);

psa_status_t esp_sha512_driver_abort(esp_sha512_context *ctx);

psa_status_t esp_sha512_driver_clone(const esp_sha512_context *source_ctx, esp_sha512_context *target_ctx);
#endif /* ESP_SHA_DRIVER_ENABLED */
