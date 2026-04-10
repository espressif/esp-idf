/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include "psa/crypto.h"
#if defined(SOC_DIG_SIGN_SUPPORTED) && defined(CONFIG_MBEDTLS_HARDWARE_RSA_DS_PERIPHERAL)
#include "psa_crypto_driver_esp_rsa_ds_contexts.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ESP_RSA_DS_DRIVER_ENABLED)
#ifndef PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#define PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#endif /* PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT */
#endif /* ESP_RSA_DS_DRIVER_ENABLED */

#define PSA_KEY_LOCATION_ESP_RSA_DS ((psa_key_location_t) 0x800003)

/* IDF-15427: ESP-PSA driver does not support persistent RSA DS keys as of now */
#if 0
/* @brief Construct a lifetime for ESP RSA DS keys with default persistence */
#define PSA_KEY_LIFETIME_ESP_RSA_DS \
    PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
        PSA_KEY_PERSISTENCE_DEFAULT, \
        PSA_KEY_LOCATION_ESP_RSA_DS)
#endif

/**
 * @brief Construct a volatile lifetime for ESP RSA DS keys
 */
#define PSA_KEY_LIFETIME_ESP_RSA_DS_VOLATILE \
    PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
        PSA_KEY_PERSISTENCE_VOLATILE, \
        PSA_KEY_LOCATION_ESP_RSA_DS)

/**
 * @brief Start the RSA DS opaque sign hash operation
 *
 * @param operation Operation context
 * @param attributes Key attributes
 * @param key_buffer Key buffer
 * @param key_buffer_size Key buffer size
 * @param alg Algorithm used in the sign operation
 * @param hash Hash to sign
 * @param hash_length Length of the hash
 * @return Status of the start operation
 *         PSA_SUCCESS if the operation was started successfully
 *         Error code if the operation was not started successfully, check psa_status_t documentation for more details.
 */
psa_status_t esp_rsa_ds_opaque_sign_hash_start(
    esp_rsa_ds_opaque_sign_hash_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length);

/**
 * @brief Complete the RSA DS opaque sign hash operation.
 *        The signature buffer should be pre-allocated with the size of the RSA key.
 *        The operation context should be initialised with the start operation.
 * @param operation Operation context
 * @param signature Signature buffer
 * @param signature_size Size of the signature buffer
 * @param signature_length Actual length of the signature
 * @return Status of the complete operation
 *         PSA_SUCCESS if the operation was completed successfully
 *         Error code if the operation was not completed successfully, check psa_status_t documentation for more details.
 */
psa_status_t esp_rsa_ds_opaque_sign_hash_complete(
    esp_rsa_ds_opaque_sign_hash_operation_t *operation,
    uint8_t *signature, size_t signature_size,
    size_t *signature_length);

/**
 * @brief Abort the RSA DS opaque sign hash operation.
 *        The operation context should be initialised with the start operation.
 * @param operation Operation context
 * @return Status of the abort operation
 *         PSA_SUCCESS if the operation was aborted successfully
 *         Error code if the operation was not aborted successfully, check psa_status_t documentation for more details.
 */
psa_status_t esp_rsa_ds_opaque_sign_hash_abort(
    esp_rsa_ds_opaque_sign_hash_operation_t *operation);

/**
 * @brief Sign the hash using the RSA DS opaque key.
 *        This is one-shot function that combines the start and complete operations.
 *
 * @param attributes Key attributes
 * @param key_buffer Key buffer
 * @param key_buffer_size Key buffer size
 * @param alg Algorithm used in the sign operation
 * @param hash Hash to sign
 * @param hash_length Length of the hash
 * @param signature Signature buffer
 * @param signature_size Size of the signature buffer
 * @param signature_length Actual length of the signature
 * @return Status of the sign operation
 *         PSA_SUCCESS if the operation was completed successfully
 *         Error code if the operation was not completed successfully, check psa_status_t documentation for more details.
 */
psa_status_t esp_rsa_ds_opaque_signature_sign_hash(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length,
    uint8_t *signature,
    size_t signature_size,
    size_t *signature_length
);

/**
 * @brief Import the RSA DS opaque key
 *        The data should be of type esp_rsa_ds_opaque_key_t and should be
 *        already initialised with DS data and efuse key id.
 *
 * @param attributes Key attributes
 * @param data Key data
 * @param data_length Key data length
 * @param key_buffer Key buffer
 * @param key_buffer_size Key buffer size
 * @param key_buffer_length Key buffer length
 * @param bits Key bits
 * @return Status of the import operation
 *         PSA_SUCCESS if the key was imported successfully
 *         Error code if the key was not imported successfully, check psa_status_t documentation for more details
 */
psa_status_t esp_rsa_ds_opaque_import_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *data,
    size_t data_length,
    uint8_t *key_buffer,
    size_t key_buffer_size,
    size_t *key_buffer_length,
    size_t *bits);

/**
 * @brief Return the size of the RSA DS opaque key in bytes
 *
 * @param key_type Key type
 * @param key_bits Key bits
 * @return Size of the RSA DS opaque key in bytes
 */
size_t esp_rsa_ds_opaque_size_function(
    psa_key_type_t key_type,
    size_t key_bits);

/**
 * @brief Set the timeout for the RSA DS session
 *
 * @param timeout_ms Timeout in milliseconds
 */
void esp_rsa_ds_opaque_set_session_timeout(int timeout_ms);

/**
 * @brief Decrypt the input using the RSA DS opaque key.
 *
 * @param attributes Key attributes
 * @param key Key buffer
 * @param key_length Key buffer size
 * @param alg Algorithm used in the decrypt operation
 * @param input Input buffer
 * @param input_length Length of the input buffer
 * @param salt Salt buffer
 * @param salt_length Length of the salt buffer
 * @param output Output buffer
 * @param output_size Size of the output buffer
 * @param output_length Actual length of the output
 * @return Status of the decrypt operation
 *         PSA_SUCCESS if the operation was completed successfully
 *         Error code if the operation was not completed successfully, check psa_status_t documentation for more details.
 */
psa_status_t esp_rsa_ds_opaque_asymmetric_decrypt(
    const psa_key_attributes_t *attributes, const uint8_t *key,
    size_t key_length, psa_algorithm_t alg, const uint8_t *input,
    size_t input_length, const uint8_t *salt, size_t salt_length,
    uint8_t *output, size_t output_size, size_t *output_length);
#ifdef __cplusplus
}
#endif
#endif /* SOC_DIG_SIGN_SUPPORTED && CONFIG_MBEDTLS_HARDWARE_RSA_DS_PERIPHERAL */
