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

/**
 * @brief Construct a persistent lifetime for ESP RSA DS keys
 */
#define PSA_KEY_LIFETIME_ESP_RSA_DS \
    PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
        PSA_KEY_PERSISTENCE_DEFAULT, \
        PSA_KEY_LOCATION_ESP_RSA_DS)

/**
 * @brief Construct a volatile lifetime for ESP RSA DS keys
 */
#define PSA_KEY_LIFETIME_ESP_RSA_DS_VOLATILE \
    PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
        PSA_KEY_PERSISTENCE_VOLATILE, \
        PSA_KEY_LOCATION_ESP_RSA_DS)

/**
 * @brief Buffer size needed to serialize an ESP-RSA DS key into the persistent
 *        storage layout used by this driver.
 *
 * The size depends on the key source: eFuse-sourced keys and Key Manager-sourced
 * keys (KM-capable SoCs only) use different inline storage structs. The source
 * is inferred from @p opaque_key — the Key Manager layout is selected when
 * @p opaque_key->key_recovery_info is non-NULL.
 *
 * @param opaque_key  User-facing opaque key (must be non-NULL).
 * @return Storage buffer size in bytes, or 0 if @p opaque_key is NULL.
 */
size_t esp_rsa_ds_persistent_key_buffer_size(const esp_rsa_ds_opaque_key_t *opaque_key);

/**
 * @brief Serialize an ESP-RSA DS key into the persistent storage layout that
 *        this driver expects when loading a persistent key from PSA storage.
 *
 * Custom PSA ITS backends that synthesize persistent RSA-DS key blobs at
 * read time can use this helper to produce the @c key_data payload, then wrap
 * it with esp_psa_its_pack_key_blob() to build the full PSA persistent key blob.
 *
 * The output layout (eFuse vs Key Manager) is selected automatically from
 * @p opaque_key, mirroring the import path. Validation of @p opaque_key is
 * performed before serialization.
 *
 * @param opaque_key  User-facing opaque key.
 * @param buf         Output buffer (caller-allocated, sized via
 *                    esp_rsa_ds_persistent_key_buffer_size()).
 * @param buf_size    Size of @p buf in bytes.
 * @param[out] out_len Bytes written to @p buf on success.
 *
 * @return PSA_SUCCESS on success
 * @return PSA_ERROR_INVALID_ARGUMENT if any required input is NULL or the
 *                                    opaque key fields are invalid
 * @return PSA_ERROR_BUFFER_TOO_SMALL if @p buf_size is insufficient
 */
psa_status_t esp_rsa_ds_format_persistent_key_buffer(const esp_rsa_ds_opaque_key_t *opaque_key,
                                                 uint8_t *buf, size_t buf_size,
                                                 size_t *out_len);

/**
 * @brief Parse an ESP-RSA DS persistent key buffer.
 *
 * Inverse of esp_rsa_ds_format_persistent_key_buffer(): validates the
 * key-storage metadata (version + source) in @p buf, then fills @p out
 * with the same opaque-key shape the format path consumes.
 *
 * The caller owns the storage for @p out, including the @c esp_ds_data_ctx_t
 * it points to (@p out->ds_data_ctx must be non-NULL before the call).
 * On success, scalar fields (@c efuse_key_id, @c rsa_length_bits) are filled
 * into the caller's @c esp_ds_data_ctx_t, and the @c esp_ds_data pointer
 * — together with @c out->key_recovery_info on KM-capable SoCs — aliases
 * into @p buf. Those pointers remain valid only for as long as @p buf is,
 * and must not be written through.
 *
 * The key source (eFuse vs Key Manager) is conveyed implicitly: on success,
 * @c out->key_recovery_info is non-NULL iff the buffer was produced from a
 * Key-Manager-backed key. This mirrors how the format path discriminates
 * via the same field.
 *
 * Custom PSA ITS backends that accept writes (translating PSA-formatted
 * blobs handed to psa_its_set() into a native storage format) can use this
 * helper after first stripping the PSA wrapper with esp_psa_its_unpack_key_blob().
 *
 * @param buf       Input buffer (as produced by esp_rsa_ds_format_persistent_key_buffer()
 *                  or written by the driver's import path).
 * @param buf_len   Length of @p buf in bytes.
 * @param[in,out] out  Opaque key to fill. @p out->ds_data_ctx must point to a
 *                  caller-allocated @c esp_ds_data_ctx_t.
 *
 * @return PSA_SUCCESS on success
 * @return PSA_ERROR_INVALID_ARGUMENT if @p buf, @p out, or @p out->ds_data_ctx
 *                                    is NULL, or @p buf_len is too small, or
 *                                    the buffer is internally inconsistent
 *                                    (e.g. invalid RSA length)
 * @return PSA_ERROR_DATA_INVALID     if the metadata version or key source is
 *                                    unrecognized, or the embedded esp_ds_data_t
 *                                    does not match the declared key length
 */
psa_status_t esp_rsa_ds_parse_persistent_key_buffer(const uint8_t *buf, size_t buf_len,
                                                    esp_rsa_ds_opaque_key_t *out);

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
 * @brief Return the storage buffer size required for an RSA DS opaque key
 *
 * For volatile keys, returns a small pointer-based storage size.
 * For persistent keys, inspects the user-facing import data to determine
 * the key source and returns the corresponding inline storage struct size.
 *
 * @param attributes Key attributes (used to check persistence)
 * @param key_type   Key type
 * @param data       Import data (user-facing esp_rsa_ds_opaque_key_t)
 * @param data_length Length of import data
 * @return Size of the storage buffer in bytes, or 0 on error
 */
size_t esp_rsa_ds_opaque_size_function(
    const psa_key_attributes_t *attributes,
    psa_key_type_t key_type,
    const uint8_t *data,
    size_t data_length);

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
