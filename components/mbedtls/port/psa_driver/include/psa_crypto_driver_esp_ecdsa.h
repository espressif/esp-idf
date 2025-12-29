/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#if defined(ESP_ECDSA_DRIVER_ENABLED)
#ifndef PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#define PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#endif

#include "psa/crypto.h"
#include "psa/crypto_types.h"
#include "soc/soc_caps.h"

#include "psa_crypto_driver_esp_ecdsa_contexts.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ESP ECDSA PSA driver location
 *
 * Vendor-specific location for ESP hardware ECDSA keys.
 * Bits 8-31 are location, using vendor flag (0x800000) + ESP vendor ID.
 */
#define PSA_KEY_LOCATION_ESP_ECDSA          ((psa_key_location_t) 0x800001)

/**
 * @brief Construct a lifetime for ESP ECDSA keys with default persistence
 */
#define PSA_KEY_LIFETIME_ESP_ECDSA \
    PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
        PSA_KEY_PERSISTENCE_DEFAULT, \
        PSA_KEY_LOCATION_ESP_ECDSA)

/**
 * @brief Construct a volatile lifetime for ESP ECDSA keys
 */
#define PSA_KEY_LIFETIME_ESP_ECDSA_VOLATILE \
    PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
        PSA_KEY_PERSISTENCE_VOLATILE, \
        PSA_KEY_LOCATION_ESP_ECDSA)

/**
 * @brief Sign a hash using ESP ECDSA opaque driver
 *
 * @param attributes        Key attributes
 * @param key_buffer        Opaque key buffer (esp_ecdsa_opaque_key_t)
 * @param key_buffer_size   Size of key buffer
 * @param alg               Signing algorithm
 * @param hash              Hash to sign
 * @param hash_length       Length of hash
 * @param signature         Output buffer for signature
 * @param signature_size    Size of signature buffer
 * @param signature_length  Actual signature length
 * @return psa_status_t
 */
psa_status_t esp_ecdsa_opaque_sign_hash(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length,
    uint8_t *signature,
    size_t signature_size,
    size_t *signature_length);

/**
 * @brief Import an ESP ECDSA key reference (not actual key material)
 *
 * This function imports an opaque reference to a key stored in eFuse or Key Manager.
 * The import data should contain the esp_ecdsa_opaque_key_t structure.
 *
 * @param attributes        Key attributes
 * @param data              Import data (esp_ecdsa_opaque_key_t)
 * @param data_length       Length of import data
 * @param key_buffer        Output buffer for opaque key
 * @param key_buffer_size   Size of output buffer
 * @param key_buffer_length Actual key buffer length
 * @param bits              Key size in bits
 * @return psa_status_t
 */
psa_status_t esp_ecdsa_opaque_import_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *data,
    size_t data_length,
    uint8_t *key_buffer,
    size_t key_buffer_size,
    size_t *key_buffer_length,
    size_t *bits);

/**
 * @brief Export the public key from an ESP ECDSA opaque key
 *
 * @param attributes        Key attributes
 * @param key_buffer        Opaque key buffer
 * @param key_buffer_size   Size of key buffer
 * @param data              Output buffer for public key
 * @param data_size         Size of output buffer
 * @param data_length       Actual public key length
 * @return psa_status_t
 */
psa_status_t esp_ecdsa_opaque_export_public_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    uint8_t *data,
    size_t data_size,
    size_t *data_length);

/**
 * @brief Get the key buffer size for an ESP ECDSA opaque key
 *
 * @param key_type          Key type
 * @param key_bits          Key size in bits
 * @return size_t           Required buffer size
 */
size_t esp_ecdsa_opaque_size_function(
    psa_key_type_t key_type,
    size_t key_bits);

/**
 * @brief Verify a hash using ESP ECDSA transparent driver
 *
 * @param attributes        Key attributes
 * @param key_buffer        Key buffer
 * @param key_buffer_size   Size of key buffer
 * @param alg               Algorithm
 * @param hash              Hash to verify
 * @param hash_length       Length of hash
 * @param signature         Signature to verify
 * @param signature_length  Length of signature
 * @return psa_status_t
 */
psa_status_t esp_ecdsa_transparent_verify_hash(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length,
    const uint8_t *signature,
    size_t signature_length);

/**
 * @brief Start a hash verification operation using ESP ECDSA transparent driver
 *
 * @param operation         Operation context
 * @param attributes        Key attributes
 * @param key_buffer        Key buffer
 * @param key_buffer_size   Size of key buffer
 * @param alg               Algorithm
 * @param hash              Hash to verify
 * @param hash_length       Length of hash
 * @param signature         Signature to verify
 * @param signature_length  Length of signature
 * @return psa_status_t
 */
psa_status_t esp_ecdsa_transparent_verify_hash_start(
    esp_ecdsa_transparent_verify_hash_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length,
    const uint8_t *signature,
    size_t signature_length);

/**
 * @brief Complete a hash verification operation using ESP ECDSA transparent driver
 *
 * @param operation         Operation context
 * @return psa_status_t
 */
psa_status_t esp_ecdsa_transparent_verify_hash_complete(
    esp_ecdsa_transparent_verify_hash_operation_t *operation);

/**
 * @brief Abort a hash verification operation using ESP ECDSA transparent driver
 *
 * @param operation         Operation context
 * @return psa_status_t
 */
psa_status_t esp_ecdsa_transparent_verify_hash_abort(
    esp_ecdsa_transparent_verify_hash_operation_t *operation);

/**
 * @brief Start a hash signing operation using ESP ECDSA opaque driver
 *
 * @param operation         Operation context
 * @param attributes        Key attributes
 * @param key_buffer        Key buffer
 * @param key_buffer_size   Size of key buffer
 * @param alg               Algorithm
 * @param hash              Hash to sign
 * @param hash_length       Length of hash
 * @return psa_status_t
 */
psa_status_t esp_ecdsa_opaque_sign_hash_start(
    esp_ecdsa_opaque_sign_hash_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length);

/**
 * @brief Complete a hash signing operation using ESP ECDSA opaque driver
 *
 * @param operation         Operation context
 * @param signature         Signature buffer
 * @param signature_size    Size of signature buffer
 * @param signature_length  Actual signature length
 * @return psa_status_t
 */
psa_status_t esp_ecdsa_opaque_sign_hash_complete(
    esp_ecdsa_opaque_sign_hash_operation_t *operation,
    uint8_t *signature, size_t signature_size,
    size_t *signature_length);

/**
 * @brief Abort a hash signing operation using ESP ECDSA opaque driver
 *
 * @param operation         Operation context
 * @return psa_status_t
 */
psa_status_t esp_ecdsa_opaque_sign_hash_abort(
    esp_ecdsa_opaque_sign_hash_operation_t *operation);

#ifdef __cplusplus
}
#endif

#endif /* ESP_ECDSA_DRIVER_ENABLED */
