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
// typedef struct esp_sha1_context esp_sha1_context;
typedef uint32_t psa_algorithm_t;
typedef int32_t psa_status_t;

psa_status_t esp_sha1_driver_compute(
    esp_sha1_context *ctx,
    const uint8_t *input,
    size_t input_length,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length);

psa_status_t esp_sha1_driver_update(
    esp_sha1_context *ctx,
    const uint8_t *input,
    size_t input_length);

psa_status_t esp_sha1_driver_finish(
    esp_sha1_context *ctx,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length);
#endif /* ESP_SHA_DRIVER_ENABLED */
