/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * ESP ECDSA PSA Crypto Transparent Sign Driver
 *
 * Signs with a plaintext (transparent) SECP-R1 key pair through the ECDSA
 * peripheral's software key source, available from the ESP32-S31 onwards.
 */

#include <stdbool.h>
#include <string.h>
#include "soc/soc_caps.h"
#include "esp_log.h"

#include "esp_efuse.h"

#include "hal/ecdsa_types.h"
#include "hal/ecdsa_hal.h"
#include "hal/ecdsa_ll.h"

#include "mbedtls/platform_util.h"

#include "psa_crypto_driver_esp_ecdsa.h"
#include "include/psa_crypto_driver_esp_ecdsa_utilities.h"
#include "sdkconfig.h"

#if defined(ESP_ECDSA_TRANSPARENT_SIGN_DRIVER_ENABLED)

static const char *TAG = "psa_crypto_driver_esp_ecdsa";

/* Bound for the zero-signature retry loop, in case the peripheral keeps
 * failing for a given input */
#define ESP_ECDSA_SIGN_MAX_ATTEMPTS     16

psa_status_t esp_ecdsa_transparent_sign_hash_start(
    esp_ecdsa_transparent_sign_hash_operation_t *operation,
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

    memset(operation, 0, sizeof(esp_ecdsa_transparent_sign_hash_operation_t));

    /* Any condition the peripheral cannot handle returns PSA_ERROR_NOT_SUPPORTED
     * so that the PSA core falls back to the builtin implementation */

    // Check if the ECDSA peripheral is supported on this chip revision
    if (!ecdsa_ll_is_supported()) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    // Check if the software key source has been permanently disabled by eFuse
    if (!esp_efuse_is_ecdsa_software_key_supported()) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (!PSA_ALG_IS_ECDSA(alg)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    // Only plaintext SECP-R1 key pairs are supported
    if (psa_get_key_type(attributes) != PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (PSA_ALG_ECDSA_IS_DETERMINISTIC(alg)) {
#if SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE && SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP
        if (!ecdsa_ll_is_deterministic_mode_supported()) {
            return PSA_ERROR_NOT_SUPPORTED;
        }
#else
        return PSA_ERROR_NOT_SUPPORTED;
#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE && SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP */
    }

    size_t key_len = PSA_BITS_TO_BYTES(psa_get_key_bits(attributes));
    esp_ecdsa_curve_t curve = esp_ecdsa_bits_to_curve(key_len);
    if (curve == ESP_ECDSA_CURVE_MAX) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (curve == ESP_ECDSA_CURVE_SECP192R1 && !esp_efuse_is_ecdsa_p192_curve_supported()) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    psa_status_t status = esp_ecdsa_validate_sha_alg(alg, curve);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* esp_ecdsa_validate_sha_alg has already pinned the hash algorithm to the curve,
     * so the expected digest length is exactly that hash's output length. PSA does not
     * force hash_length to match the algorithm, so the length is still checked here. */
    if (hash_length != PSA_HASH_LENGTH(PSA_ALG_SIGN_GET_HASH(alg))) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* A transparent SECP-R1 key pair is stored as the raw private key in big-endian
     * format. The PSA core has already validated 1 <= key < n at import time. */
    if (key_buffer_size != key_len) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    operation->alg = alg;
    operation->curve = curve;
    operation->key_len = key_len;
    esp_ecdsa_change_endianness(key_buffer, operation->key, key_len);
    esp_ecdsa_change_endianness(hash, operation->sha, key_len);

    return PSA_SUCCESS;
}

psa_status_t esp_ecdsa_transparent_sign_hash_complete(
    esp_ecdsa_transparent_sign_hash_operation_t *operation,
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

    ecdsa_curve_t hal_curve = esp_ecdsa_curve_to_hal_curve(operation->curve);
    if (hal_curve == (ecdsa_curve_t)-1) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    ecdsa_sign_type_t k_type = ECDSA_K_TYPE_TRNG;

#if SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
    if (PSA_ALG_ECDSA_IS_DETERMINISTIC(operation->alg)) {
        k_type = ECDSA_K_TYPE_DETERMINISITIC;
    }
#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE */

    uint8_t zeroes[MAX_ECDSA_COMPONENT_LEN] = {0};

    ecdsa_hal_config_t conf = {
        .mode = ECDSA_MODE_SIGN_GEN,
        .curve = hal_curve,
        .sha_mode = ECDSA_Z_USER_PROVIDED,
        .sign_type = k_type,
        .use_sw_key = true,
        .sw_key = operation->key,
    };

    esp_ecdsa_acquire_hardware();

    bool process_again = false;
    int attempts = 0;

    do {
        ecdsa_hal_gen_signature(&conf, operation->sha, operation->r, operation->s, component_len);

        process_again = !ecdsa_hal_get_operation_result()
                        || !memcmp(operation->r, zeroes, component_len)
                        || !memcmp(operation->s, zeroes, component_len);
    } while (process_again && ++attempts < ESP_ECDSA_SIGN_MAX_ATTEMPTS);

    esp_ecdsa_release_hardware();

    if (process_again) {
        ESP_LOGE(TAG, "Failed to generate an ECDSA signature using the software key");
        return PSA_ERROR_GENERIC_ERROR;
    }

    // Convert r and s from little-endian to big-endian and copy to output
    esp_ecdsa_change_endianness(operation->r, signature, component_len);
    esp_ecdsa_change_endianness(operation->s, signature + component_len, component_len);

    *signature_length = 2 * component_len;

    return PSA_SUCCESS;
}

psa_status_t esp_ecdsa_transparent_sign_hash_abort(esp_ecdsa_transparent_sign_hash_operation_t *operation)
{
    if (operation) {
        /* Zeroizes the plaintext private key copy held in the operation context */
        mbedtls_platform_zeroize(operation, sizeof(esp_ecdsa_transparent_sign_hash_operation_t));
    }
    return PSA_SUCCESS;
}

psa_status_t esp_ecdsa_transparent_sign_hash(
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
    esp_ecdsa_transparent_sign_hash_operation_t operation;

    psa_status_t status = esp_ecdsa_transparent_sign_hash_start(&operation, attributes, key_buffer,
                                                                key_buffer_size, alg, hash, hash_length);
    if (status == PSA_SUCCESS) {
        status = esp_ecdsa_transparent_sign_hash_complete(&operation, signature, signature_size, signature_length);
    }

    /* Always scrub the plaintext private key copy from the stack, on success and failure. */
    esp_ecdsa_transparent_sign_hash_abort(&operation);
    return status;
}
#endif /* ESP_ECDSA_TRANSPARENT_SIGN_DRIVER_ENABLED */
