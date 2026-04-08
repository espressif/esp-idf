/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "psa/crypto.h"
#include "psa/crypto_types.h"

#include "psa_crypto_driver_secure_element_contexts.h"

#if defined(SECURE_ELEMENT_DRIVER_ENABLED) || defined(CONFIG_MBEDTLS_SECURE_ELEMENT_DRIVER_ENABLED)
#ifndef PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#define PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Secure element PSA driver location
 *
 * Vendor-specific location for secure element keys.
 * Bits 8-31 are location, using vendor flag (0x800000) + SE vendor ID.
 */
#define PSA_KEY_LOCATION_SECURE_ELEMENT          ((psa_key_location_t) 0x800004)

/**
 * @brief Construct a lifetime for secure element keys with default persistence
 */
#define PSA_KEY_LIFETIME_SECURE_ELEMENT \
    PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
        PSA_KEY_PERSISTENCE_DEFAULT, \
        PSA_KEY_LOCATION_SECURE_ELEMENT)

/**
 * @brief Construct a volatile lifetime for secure element keys
 */
#define PSA_KEY_LIFETIME_SECURE_ELEMENT_VOLATILE \
    PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
        PSA_KEY_PERSISTENCE_VOLATILE, \
        PSA_KEY_LOCATION_SECURE_ELEMENT)

/*
 * Callback typedefs for secure element operations.
 *
 * These allow the SE-specific backend (e.g. esp-cryptoauthlib for ATECC608A)
 * to register its implementation at runtime without any build-time dependency.
 */

/**
 * @brief Callback to sign a hash using the secure element
 *
 * @param slot_id   Slot containing the private key
 * @param hash      Hash to sign
 * @param hash_len  Length of hash (e.g. 32 for SHA-256)
 * @param sig       Output buffer for raw signature (R || S)
 * @param sig_size  Size of sig buffer
 * @param sig_len   Actual signature length written
 * @return psa_status_t
 */
typedef psa_status_t (*secure_element_sign_cb_t)(uint8_t slot_id, const uint8_t *hash, size_t hash_len,
    uint8_t *sig, size_t sig_size, size_t *sig_len);

/**
 * @brief Callback to export the public key from a secure element slot
 *
 * @param slot_id       Slot containing the key pair
 * @param pubkey        Output buffer for raw public key (X || Y)
 * @param pubkey_size   Size of pubkey buffer
 * @param pubkey_len    Actual public key length written
 * @return psa_status_t
 */
typedef psa_status_t (*secure_element_export_pubkey_cb_t)(uint8_t slot_id, uint8_t *pubkey,
    size_t pubkey_size, size_t *pubkey_len);

/**
 * @brief Callback to verify a hash signature using the secure element
 *
 * @param hash          Hash that was signed
 * @param hash_len      Length of hash
 * @param sig           Raw signature (R || S)
 * @param sig_len       Length of signature
 * @param pubkey        Raw public key (X || Y)
 * @param pubkey_len    Length of public key
 * @param is_verified   Output: true if signature is valid
 * @return psa_status_t
 */
typedef psa_status_t (*secure_element_verify_cb_t)(const uint8_t *hash, size_t hash_len,
    const uint8_t *sig, size_t sig_len, const uint8_t *pubkey, size_t pubkey_len, bool *is_verified);

/**
 * @brief Secure element callback table
 *
 * The algorithm, key_type, and key_bits fields declare what the SE supports.
 * The driver validates incoming requests against these and returns
 * PSA_ERROR_NOT_SUPPORTED for anything that doesn't match, allowing the
 * PSA framework to fall back to software.
 */
typedef struct {
    secure_element_sign_cb_t          sign;           /**< Sign hash callback (NULL if not supported) */
    secure_element_export_pubkey_cb_t export_pubkey;   /**< Export public key callback (NULL if not supported) */
    secure_element_verify_cb_t        verify;          /**< Verify hash callback (NULL if not supported) */
    psa_algorithm_t                   algorithm;       /**< Supported algorithm, e.g. PSA_ALG_ECDSA(PSA_ALG_SHA_256) */
    psa_key_type_t                    key_type;        /**< Supported key type, e.g. PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1) */
    size_t                            key_bits;        /**< Supported key size in bits, e.g. 256 */
} secure_element_callbacks_t;

/**
 * @brief Register secure element callbacks
 *
 * Must be called once during application initialization, before any PSA
 * operations targeting PSA_KEY_LOCATION_SECURE_ELEMENT. Uses atomic
 * compare-and-swap so only the first call succeeds.
 *
 * @param callbacks  Pointer to callback table (must remain valid for program lifetime)
 * @return PSA_SUCCESS on success
 * @return PSA_ERROR_BAD_STATE if callbacks were already registered
 * @return PSA_ERROR_INVALID_ARGUMENT if callbacks is NULL
 */
psa_status_t secure_element_register_callbacks(const secure_element_callbacks_t *callbacks);

/**
 * @brief Sign a hash using secure element opaque driver
 */
psa_status_t secure_element_opaque_sign_hash(
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
 * @brief Import a secure element key reference (not actual key material)
 */
psa_status_t secure_element_opaque_import_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *data,
    size_t data_length,
    uint8_t *key_buffer,
    size_t key_buffer_size,
    size_t *key_buffer_length,
    size_t *bits);

/**
 * @brief Export the public key from a secure element opaque key
 */
psa_status_t secure_element_opaque_export_public_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    uint8_t *data,
    size_t data_size,
    size_t *data_length);

/**
 * @brief Get the key buffer size for a secure element opaque key
 */
size_t secure_element_opaque_size_function(
    psa_key_type_t key_type,
    size_t key_bits);

/**
 * @brief Verify a hash using secure element transparent driver
 */
psa_status_t secure_element_transparent_verify_hash(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length,
    const uint8_t *signature,
    size_t signature_length);

/**
 * @brief Start a hash verification operation
 */
psa_status_t secure_element_transparent_verify_hash_start(
    secure_element_transparent_verify_hash_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length,
    const uint8_t *signature,
    size_t signature_length);

/**
 * @brief Complete a hash verification operation
 */
psa_status_t secure_element_transparent_verify_hash_complete(
    secure_element_transparent_verify_hash_operation_t *operation);

/**
 * @brief Abort a hash verification operation
 */
psa_status_t secure_element_transparent_verify_hash_abort(
    secure_element_transparent_verify_hash_operation_t *operation);

/**
 * @brief Start a hash signing operation using opaque driver
 */
psa_status_t secure_element_opaque_sign_hash_start(
    secure_element_opaque_sign_hash_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length);

/**
 * @brief Complete a hash signing operation using opaque driver
 */
psa_status_t secure_element_opaque_sign_hash_complete(
    secure_element_opaque_sign_hash_operation_t *operation,
    uint8_t *signature, size_t signature_size,
    size_t *signature_length);

/**
 * @brief Abort a hash signing operation using opaque driver
 */
psa_status_t secure_element_opaque_sign_hash_abort(
    secure_element_opaque_sign_hash_operation_t *operation);

#ifdef __cplusplus
}
#endif

#endif /* SECURE_ELEMENT_DRIVER_ENABLED || CONFIG_MBEDTLS_SECURE_ELEMENT_DRIVER_ENABLED */
