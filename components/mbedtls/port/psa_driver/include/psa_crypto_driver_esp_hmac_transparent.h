/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "psa/crypto.h"
#include "psa_crypto_driver_esp_hmac_transparent_contexts.h"

#if defined(ESP_HMAC_TRANSPARENT_DRIVER_ENABLED)
#ifndef PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#define PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#endif

#ifdef __cplusplus
extern "C" {
#endif

psa_status_t esp_hmac_compute_transparent(const psa_key_attributes_t *attributes,
                                        const uint8_t *key_buffer,
                                        size_t key_buffer_size,
                                        psa_algorithm_t alg,
                                        const uint8_t *input,
                                        size_t input_length,
                                        uint8_t *mac,
                                        size_t mac_size,
                                        size_t *mac_length);

psa_status_t esp_hmac_abort_transparent(esp_hmac_transparent_operation_t *esp_hmac_ctx);

psa_status_t esp_hmac_setup_transparent(esp_hmac_transparent_operation_t *esp_hmac_ctx,
                                        const psa_key_attributes_t *attributes,
                                        const uint8_t *key_buffer,
                                        size_t key_buffer_size,
                                        psa_algorithm_t alg);

psa_status_t esp_hmac_update_transparent(esp_hmac_transparent_operation_t *esp_hmac_ctx,
                                        const uint8_t *data,
                                        size_t data_length);

psa_status_t esp_hmac_finish_transparent(esp_hmac_transparent_operation_t *esp_hmac_ctx,
                                        uint8_t *mac,
                                        size_t mac_size,
                                        size_t *mac_length);

psa_status_t esp_hmac_verify_finish_transparent(esp_hmac_transparent_operation_t *esp_hmac_ctx,
                                                const uint8_t *mac,
                                                size_t mac_length);

#ifdef __cplusplus
}
#endif

#endif /* ESP_HMAC_TRANSPARENT_DRIVER_ENABLED */
