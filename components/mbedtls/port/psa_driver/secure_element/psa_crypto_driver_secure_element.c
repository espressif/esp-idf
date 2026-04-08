/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Generic Secure Element PSA Crypto Driver
 *
 * This driver provides PSA Crypto API integration for external secure elements
 * (e.g., ATECC608A). The SE-specific operations are dispatched through
 * runtime-registered callbacks, removing any build-time dependency on a
 * particular SE library.
 *
 * The callbacks struct declares which algorithm/key_type/key_bits the SE
 * supports. The driver validates each request against these and returns
 * PSA_ERROR_NOT_SUPPORTED for mismatches, letting PSA fall back to software.
 */

#include <string.h>
#include <stdatomic.h>
#include "sdkconfig.h"

#ifdef SECURE_ELEMENT_DRIVER_ENABLED

#include "esp_log.h"
#include "psa_crypto_driver_secure_element.h"

static const char *TAG = "psa_crypto_driver_secure_element";

#define UNCOMPRESSED_POINT_FORMAT 0x04

/* Runtime-registered SE callbacks (set once via secure_element_register_callbacks) */
static const secure_element_callbacks_t *s_se_callbacks = NULL;

psa_status_t secure_element_register_callbacks(const secure_element_callbacks_t *callbacks)
{
    if (callbacks == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (callbacks->key_bits == 0) {
        ESP_LOGE(TAG, "key_bits must be set in callbacks");
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Atomic compare-and-swap: only the first registration succeeds */
    const secure_element_callbacks_t *expected = NULL;
    if (!atomic_compare_exchange_strong((volatile _Atomic(const secure_element_callbacks_t *) *)&s_se_callbacks,
                                       &expected, callbacks)) {
        ESP_LOGE(TAG, "Secure element callbacks already registered");
        return PSA_ERROR_BAD_STATE;
    }

    return PSA_SUCCESS;
}

/**
 * @brief Get the registered callbacks, or NULL if not yet registered
 */
static inline const secure_element_callbacks_t *se_get_callbacks(void)
{
    return s_se_callbacks;
}

/**
 * @brief Check if a request matches the registered SE capabilities
 *
 * Compares the algorithm and key type from the request against what the SE
 * declared at registration time. Returns PSA_ERROR_NOT_SUPPORTED on mismatch.
 */
static psa_status_t validate_request(psa_algorithm_t alg, const psa_key_attributes_t *attributes)
{
    const secure_element_callbacks_t *cbs = se_get_callbacks();
    if (!cbs) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* Check algorithm family matches (e.g., both are ECDSA) */
    psa_algorithm_t registered_alg = cbs->algorithm;

    /* Use PSA_ALG_SIGN_GET_HASH to compare base algorithm ignoring hash */
    if (PSA_ALG_IS_ECDSA(registered_alg)) {
        if (!PSA_ALG_IS_ECDSA(alg)) {
            return PSA_ERROR_NOT_SUPPORTED;
        }
        /* If registered with a specific hash, check it matches */
        psa_algorithm_t reg_hash = PSA_ALG_SIGN_GET_HASH(registered_alg);
        if (reg_hash != PSA_ALG_ANY_HASH && reg_hash != PSA_ALG_SIGN_GET_HASH(alg)) {
            return PSA_ERROR_NOT_SUPPORTED;
        }
    } else if (PSA_ALG_IS_RSA_PKCS1V15_SIGN(registered_alg)) {
        if (!PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg)) {
            return PSA_ERROR_NOT_SUPPORTED;
        }
    } else if (registered_alg != alg) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* Check key type matches */
    if (attributes) {
        psa_key_type_t req_type = psa_get_key_type(attributes);
        /* Accept both key pair and public key for the same family */
        psa_key_type_t reg_base = PSA_KEY_TYPE_IS_KEY_PAIR(cbs->key_type)
                                  ? PSA_KEY_TYPE_KEY_PAIR_OF_PUBLIC_KEY(PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(cbs->key_type))
                                  : cbs->key_type;
        psa_key_type_t req_base = PSA_KEY_TYPE_IS_KEY_PAIR(req_type)
                                  ? PSA_KEY_TYPE_KEY_PAIR_OF_PUBLIC_KEY(PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(req_type))
                                  : req_type;
        if (reg_base != req_base) {
            return PSA_ERROR_NOT_SUPPORTED;
        }

        /* Check key size matches */
        size_t req_bits = psa_get_key_bits(attributes);
        if (req_bits != 0 && req_bits != cbs->key_bits) {
            return PSA_ERROR_NOT_SUPPORTED;
        }
    }

    return PSA_SUCCESS;
}

/* Transparent verify operations */
psa_status_t secure_element_transparent_verify_hash_start(
    secure_element_transparent_verify_hash_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length,
    const uint8_t *signature,
    size_t signature_length)
{
    if (!operation || !attributes || !key_buffer || !hash || !signature) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Validate against registered SE capabilities */
    psa_status_t status = validate_request(alg, attributes);
    if (status != PSA_SUCCESS) {
        return status;
    }

    size_t key_len = PSA_BITS_TO_BYTES(psa_get_key_bits(attributes));
    if (key_len == 0 || key_len > SECURE_ELEMENT_MAX_KEY_BYTES) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Verify key buffer can hold the uncompressed public key (0x04 || X || Y) */
    if (key_buffer_size < 1 + 2 * key_len) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Validate hash length matches key component length */
    if (hash_length != key_len) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (signature_length != 2 * key_len) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    /* Handle public key format - must be uncompressed (0x04 || X || Y) */
    if (key_buffer[0] != UNCOMPRESSED_POINT_FORMAT) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_buffer_size != 2 * key_len + 1) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    memset(operation, 0, sizeof(secure_element_transparent_verify_hash_operation_t));
    operation->key_len = key_len;
    operation->sha_len = hash_length;

    /* Big-endian format, matching PSA format */
    memcpy(operation->sha, hash, key_len);
    memcpy(operation->r, signature, key_len);
    memcpy(operation->s, signature + key_len, key_len);

    /* Extract public key coordinates (skip 0x04 format byte) */
    memcpy(operation->qx, key_buffer + 1, key_len);
    memcpy(operation->qy, key_buffer + 1 + key_len, key_len);

    return PSA_SUCCESS;
}

psa_status_t secure_element_transparent_verify_hash_complete(secure_element_transparent_verify_hash_operation_t *operation)
{
    if (!operation) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    const secure_element_callbacks_t *cbs = se_get_callbacks();
    if (!cbs || !cbs->verify) {
        ESP_LOGE(TAG, "Secure element verify callback not registered");
        return PSA_ERROR_NOT_SUPPORTED;
    }

    size_t key_len = operation->key_len;

    /* Construct public key (X || Y) */
    uint8_t pubkey[2 * SECURE_ELEMENT_MAX_KEY_BYTES];
    memcpy(pubkey, operation->qx, key_len);
    memcpy(pubkey + key_len, operation->qy, key_len);

    /* Construct signature (R || S) */
    uint8_t sig[2 * SECURE_ELEMENT_MAX_KEY_BYTES];
    memcpy(sig, operation->r, key_len);
    memcpy(sig + key_len, operation->s, key_len);

    /* Verify using registered SE callback */
    bool is_verified = false;
    psa_status_t status = cbs->verify(operation->sha, operation->sha_len,
                                      sig, 2 * key_len,
                                      pubkey, 2 * key_len,
                                      &is_verified);

    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "SE verify callback failed: 0x%04x", (unsigned)status);
        return status;
    }

    if (!is_verified) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    return PSA_SUCCESS;
}

psa_status_t secure_element_transparent_verify_hash_abort(secure_element_transparent_verify_hash_operation_t *operation)
{
    if (operation) {
        memset(operation, 0, sizeof(secure_element_transparent_verify_hash_operation_t));
    }
    return PSA_SUCCESS;
}

psa_status_t secure_element_transparent_verify_hash(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length,
    const uint8_t *signature,
    size_t signature_length)
{
    secure_element_transparent_verify_hash_operation_t operation;

    psa_status_t status = secure_element_transparent_verify_hash_start(
        &operation, attributes, key_buffer, key_buffer_size,
        alg, hash, hash_length, signature, signature_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = secure_element_transparent_verify_hash_complete(&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return secure_element_transparent_verify_hash_abort(&operation);
}

/* Opaque sign operations */
psa_status_t secure_element_opaque_import_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *data,
    size_t data_length,
    uint8_t *key_buffer,
    size_t key_buffer_size,
    size_t *key_buffer_length,
    size_t *bits)
{
    if (!attributes || !data || data_length < 1 || !key_buffer || !key_buffer_length || !bits) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_buffer_size < sizeof(secure_element_opaque_key_t) || data_length < sizeof(secure_element_opaque_key_t)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    /* Validate the key attributes match what the SE supports */
    const secure_element_callbacks_t *cbs = se_get_callbacks();
    if (!cbs) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    psa_status_t status = validate_request(psa_get_key_algorithm(attributes), attributes);
    if (status != PSA_SUCCESS) {
        return status;
    }

    memcpy(key_buffer, data, sizeof(secure_element_opaque_key_t));
    *key_buffer_length = sizeof(secure_element_opaque_key_t);
    *bits = psa_get_key_bits(attributes);
    return PSA_SUCCESS;
}

psa_status_t secure_element_opaque_sign_hash_start(
    secure_element_opaque_sign_hash_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length)
{
    if (!operation || !attributes || !key_buffer || !hash) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_buffer_size < sizeof(secure_element_opaque_key_t)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Validate against registered SE capabilities */
    psa_status_t status = validate_request(alg, attributes);
    if (status != PSA_SUCCESS) {
        return status;
    }

    size_t component_len = PSA_BITS_TO_BYTES(psa_get_key_bits(attributes));
    if (component_len == 0 || component_len > SECURE_ELEMENT_MAX_KEY_BYTES) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Validate hash length matches key component length */
    if (hash_length != component_len) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    memset(operation, 0, sizeof(secure_element_opaque_sign_hash_operation_t));
    operation->key_len = component_len;
    memcpy(operation->sha, hash, component_len);
    operation->opaque_key = (secure_element_opaque_key_t *) key_buffer;
    operation->alg = alg;
    operation->sha_len = hash_length;

    return PSA_SUCCESS;
}

psa_status_t secure_element_opaque_sign_hash_complete(
    secure_element_opaque_sign_hash_operation_t *operation,
    uint8_t *signature, size_t signature_size,
    size_t *signature_length)
{
    if (!operation || !signature || !signature_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    size_t component_len = operation->key_len;

    if (signature_size < 2 * component_len) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    const secure_element_callbacks_t *cbs = se_get_callbacks();
    if (!cbs || !cbs->sign) {
        ESP_LOGE(TAG, "Secure element sign callback not registered");
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* Sign using registered SE callback */
    uint8_t sig[2 * SECURE_ELEMENT_MAX_KEY_BYTES];
    size_t sig_len = 0;
    psa_status_t status = cbs->sign(operation->opaque_key->slot_id,
                                    operation->sha, operation->sha_len,
                                    sig, sizeof(sig), &sig_len);

    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "SE sign callback failed: 0x%04x", (unsigned)status);
        return status;
    }

    /* Copy signature to output (R || S format, big-endian - matches PSA) */
    memcpy(signature, sig, 2 * component_len);
    *signature_length = 2 * component_len;

    return PSA_SUCCESS;
}

psa_status_t secure_element_opaque_sign_hash_abort(secure_element_opaque_sign_hash_operation_t *operation)
{
    if (operation) {
        memset(operation, 0, sizeof(secure_element_opaque_sign_hash_operation_t));
    }
    return PSA_SUCCESS;
}

psa_status_t secure_element_opaque_sign_hash(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length,
    uint8_t *signature,
    size_t signature_size,
    size_t *signature_length)
{
    secure_element_opaque_sign_hash_operation_t operation;

    psa_status_t status = secure_element_opaque_sign_hash_start(
        &operation, attributes, key_buffer, key_buffer_size, alg, hash, hash_length);
    if (status != PSA_SUCCESS) {
        secure_element_opaque_sign_hash_abort(&operation);
        return status;
    }

    status = secure_element_opaque_sign_hash_complete(&operation, signature, signature_size, signature_length);
    if (status != PSA_SUCCESS) {
        secure_element_opaque_sign_hash_abort(&operation);
        return status;
    }

    return secure_element_opaque_sign_hash_abort(&operation);
}

psa_status_t secure_element_opaque_export_public_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    uint8_t *data,
    size_t data_size,
    size_t *data_length)
{
    if (!attributes || !key_buffer || !data || !data_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_buffer_size < sizeof(secure_element_opaque_key_t)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    const secure_element_opaque_key_t *opaque_key = (const secure_element_opaque_key_t *) key_buffer;

    const secure_element_callbacks_t *cbs = se_get_callbacks();
    if (!cbs || !cbs->export_pubkey) {
        ESP_LOGE(TAG, "Secure element export_pubkey callback not registered");
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* Get key length from registered capabilities */
    size_t key_len = PSA_BITS_TO_BYTES(cbs->key_bits);

    /* Need 1 byte for format + key_len bytes for x + key_len bytes for y */
    size_t required_size = 1 + (2 * key_len);
    if (data_size < required_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    /* Export public key using registered SE callback */
    uint8_t pubkey[2 * SECURE_ELEMENT_MAX_KEY_BYTES];
    size_t pubkey_len = 0;
    psa_status_t status = cbs->export_pubkey(opaque_key->slot_id, pubkey, sizeof(pubkey), &pubkey_len);

    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "SE export_pubkey callback failed: 0x%04x", (unsigned)status);
        return status;
    }

    /* Format: uncompressed point (0x04 followed by x and y coordinates) */
    data[0] = UNCOMPRESSED_POINT_FORMAT;
    memcpy(data + 1, pubkey, key_len);               /* X coordinate */
    memcpy(data + 1 + key_len, pubkey + key_len, key_len); /* Y coordinate */

    *data_length = required_size;

    return PSA_SUCCESS;
}

size_t secure_element_opaque_size_function(
    psa_key_type_t key_type,
    size_t key_bits)
{
    (void)key_type;
    (void)key_bits;

    /* Opaque keys always use the same size structure */
    return sizeof(secure_element_opaque_key_t);
}

#endif /* SECURE_ELEMENT_DRIVER_ENABLED */
