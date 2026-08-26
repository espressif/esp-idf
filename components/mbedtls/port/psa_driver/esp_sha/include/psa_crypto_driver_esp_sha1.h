/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#pragma once

#include <stdint.h>
#include <stddef.h>

/* Forward declarations to avoid circular dependencies */
typedef uint32_t psa_algorithm_t;
typedef int32_t psa_status_t;

psa_status_t esp_sha1_driver_compute(
    esp_sha1_context *ctx,
    const uint8_t *input,
    size_t input_length,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length);

int esp_sha1_starts(esp_sha1_context *ctx);

psa_status_t esp_sha1_driver_update(
    esp_sha1_context *ctx,
    const uint8_t *input,
    size_t input_length);

psa_status_t esp_sha1_driver_finish(
    esp_sha1_context *ctx,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length);

psa_status_t esp_sha1_driver_abort(esp_sha1_context *ctx);

psa_status_t esp_sha1_driver_clone(const esp_sha1_context *source_ctx, esp_sha1_context *target_ctx);
