/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "psa/crypto.h"
#include "psa_crypto_driver_esp_hmac_opaque_contexts.h"

#if defined(ESP_HMAC_OPAQUE_DRIVER_ENABLED)
#ifndef PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#define PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ESP HMAC opaque PSA driver location
 *
 * Vendor-specific location for ESP hardware HMAC keys.
 * Bits 8-31 are location, using vendor flag (0x800000) + ESP vendor ID.
 */
#define PSA_KEY_LOCATION_ESP_HMAC          ((psa_key_location_t) 0x800002)

/**
 * @brief Construct a lifetime for ESP HMAC keys with default persistence
 */
#define PSA_KEY_LIFETIME_ESP_HMAC \
    PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
        PSA_KEY_PERSISTENCE_DEFAULT, \
        PSA_KEY_LOCATION_ESP_HMAC)

/**
 * @brief Construct a volatile lifetime for ESP HMAC keys
 */
#define PSA_KEY_LIFETIME_ESP_HMAC_VOLATILE \
    PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
        PSA_KEY_PERSISTENCE_VOLATILE, \
        PSA_KEY_LOCATION_ESP_HMAC)

/**
 * @brief Import an ESP HMAC key reference (not actual key material)
 *
 * This function imports an opaque reference to a key stored in eFuse or Key Manager.
 * The import data should contain the esp_hmac_opaque_key_t structure.
 *
 * @param attributes        Key attributes
 * @param data              Import data (esp_hmac_opaque_key_t)
 * @param data_length       Length of import data
 * @param key_buffer        Output buffer for opaque key
 * @param key_buffer_size   Size of output buffer
 * @param key_buffer_length Actual key buffer length
 * @param bits              Key size in bits
 * @return psa_status_t
 */
psa_status_t esp_hmac_import_key_opaque(const psa_key_attributes_t *attributes,
                                        const uint8_t *data,
                                        size_t data_length,
                                        uint8_t *key_buffer,
                                        size_t key_buffer_size,
                                        size_t *key_buffer_length,
                                        size_t *bits);

size_t esp_hmac_opaque_size_function(psa_key_type_t key_type, size_t key_bits);

psa_status_t esp_hmac_compute_opaque(const psa_key_attributes_t *attributes,
                                    const uint8_t *key_buffer,
                                    size_t key_buffer_size,
                                    psa_algorithm_t alg,
                                    const uint8_t *input,
                                    size_t input_length,
                                    uint8_t *mac,
                                    size_t mac_size,
                                    size_t *mac_length);

psa_status_t esp_hmac_abort_opaque(esp_hmac_opaque_operation_t *esp_hmac_ctx);

psa_status_t esp_hmac_setup_opaque(esp_hmac_opaque_operation_t *esp_hmac_ctx,
                                const psa_key_attributes_t *attributes,
                                const uint8_t *key_buffer,
                                size_t key_buffer_size,
                                psa_algorithm_t alg);

psa_status_t esp_hmac_update_opaque(esp_hmac_opaque_operation_t *esp_hmac_ctx,
                                    const uint8_t *data,
                                    size_t data_length);

psa_status_t esp_hmac_finish_opaque(esp_hmac_opaque_operation_t *esp_hmac_ctx,
                                    uint8_t *mac,
                                    size_t mac_size,
                                    size_t *mac_length);

psa_status_t esp_hmac_verify_finish_opaque(esp_hmac_opaque_operation_t *esp_hmac_ctx,
                                        const uint8_t *mac,
                                        size_t mac_length);

#ifdef __cplusplus
}
#endif

#endif /* ESP_HMAC_OPAQUE_DRIVER_ENABLED */
