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
#include "esp_fault.h"
#include "esp_efuse.h"

#if SOC_ECC_SUPPORTED
#include "ecc_impl.h"
#endif // SOC_ECC_SUPPORTED

#if SOC_ECDSA_SUPPORTED
#include "hal/ecdsa_types.h"
#include "hal/ecdsa_hal.h"
#include "hal/ecdsa_ll.h"
#endif // SOC_ECDSA_SUPPORTED

#include "mbedtls/platform_util.h"

#include "psa_crypto_driver_esp_ecdsa.h"
#include "include/psa_crypto_driver_esp_ecdsa_utilities.h"
#include "psa_crypto_driver_wrappers_no_static.h"
#include "sdkconfig.h"

#if defined(ESP_ECDSA_TRANSPARENT_SIGN_DRIVER_ENABLED) || defined(ESP_ECDSA_VERIFY_DRIVER_ENABLED)
static const char *TAG = "psa_crypto_driver_esp_ecdsa";
#endif // (ESP_ECDSA_TRANSPARENT_SIGN_DRIVER_ENABLED || ESP_ECDSA_VERIFY_DRIVER_ENABLED)

#if defined(ESP_ECDSA_TRANSPARENT_SIGN_DRIVER_ENABLED)
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

    /* Hash algorithm, eFuse curve and digest length checks shared with the opaque
     * driver. Every failure falls back to the builtin implementation. */
    if (esp_ecdsa_check_sign_request(alg, curve, hash_length) != PSA_SUCCESS) {
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

    ecdsa_hal_config_t conf = {
        .mode = ECDSA_MODE_SIGN_GEN,
        .curve = hal_curve,
        .sha_mode = ECDSA_Z_USER_PROVIDED,
        .sign_type = k_type,
        .use_sw_key = true,
        .sw_key = operation->key,
    };

    psa_status_t status = esp_ecdsa_hw_sign(&conf, operation->sha, operation->r, operation->s,
                                            component_len, ESP_ECDSA_SIGN_MAX_ATTEMPTS, signature);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to generate an ECDSA signature using the software key");
        return status;
    }

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

#ifdef ESP_ECDSA_VERIFY_DRIVER_ENABLED

#define ECDSA_UNCOMPRESSED_POINT_FORMAT 0x04

static mbedtls_ecp_group_id ecdsa_curve_to_mbedtls_group(esp_ecdsa_curve_t curve)
{
    switch (curve) {
        case ESP_ECDSA_CURVE_SECP256R1: return MBEDTLS_ECP_DP_SECP256R1;
#if SOC_ECDSA_SUPPORT_CURVE_P384
        case ESP_ECDSA_CURVE_SECP384R1: return MBEDTLS_ECP_DP_SECP384R1;
#endif
        default:                        return MBEDTLS_ECP_DP_NONE;
    }
}

static psa_status_t check_ecdsa_signature_range(const uint8_t *signature, size_t key_len,
                                                esp_ecdsa_curve_t curve)
{
    mbedtls_ecp_group_id grp_id = ecdsa_curve_to_mbedtls_group(curve);
    if (grp_id == MBEDTLS_ECP_DP_NONE) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    mbedtls_ecp_group grp;
    mbedtls_mpi r, s;
    mbedtls_ecp_group_init(&grp);
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    psa_status_t status = PSA_ERROR_INVALID_SIGNATURE;

    if (mbedtls_ecp_group_load(&grp, grp_id) != 0) {
        status = PSA_ERROR_GENERIC_ERROR;
        goto cleanup;
    }
    if (mbedtls_mpi_read_binary(&r, signature, key_len) != 0 ||
        mbedtls_mpi_read_binary(&s, signature + key_len, key_len) != 0) {
        status = PSA_ERROR_GENERIC_ERROR;
        goto cleanup;
    }

    /* 1 <= scalar <= n-1: that is, scalar > 0 and scalar < n. */
    #define RANGE_OK   0x6A6A6A6AU
    #define RANGE_FAIL 0x95959595U
    volatile uint32_t verdict = RANGE_FAIL;
    if (mbedtls_mpi_cmp_int(&r, 0) > 0 &&
        mbedtls_mpi_cmp_mpi(&r, &grp.N) < 0 &&
        mbedtls_mpi_cmp_int(&s, 0) > 0 &&
        mbedtls_mpi_cmp_mpi(&s, &grp.N) < 0) {
        verdict = RANGE_OK;
    }
    if (verdict != RANGE_OK) {
        goto cleanup;
    }
    ESP_FAULT_ASSERT(verdict == RANGE_OK);
    #undef RANGE_OK
    #undef RANGE_FAIL

    status = PSA_SUCCESS;

cleanup:
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    mbedtls_ecp_group_free(&grp);
    return status;
}

psa_status_t esp_ecdsa_transparent_verify_hash_start(
    esp_ecdsa_transparent_verify_hash_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length,
    const uint8_t *signature,
    size_t signature_length)
{
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;

    // Check if the ECDSA peripheral is supported on this chip revision
    if (!ecdsa_ll_is_supported()) {
        ESP_LOGE(TAG, "ECDSA peripheral not supported on this chip revision");
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (!operation || !attributes || !key_buffer || !hash || !signature) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    // Check if ECDSA algorithm
    if (!PSA_ALG_IS_ECDSA(alg)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* HW only implements SECP_R1; reject any other ECC family up front so
     * we never derive the curve from key_bits alone. */
    psa_key_type_t key_type = psa_get_key_type(attributes);
    if (!PSA_KEY_TYPE_IS_ECC(key_type) ||
        PSA_KEY_TYPE_ECC_GET_FAMILY(key_type) != PSA_ECC_FAMILY_SECP_R1) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    size_t key_len = PSA_BITS_TO_BYTES(psa_get_key_bits(attributes));
    esp_ecdsa_curve_t curve = esp_ecdsa_bits_to_curve(key_len);
    if (curve == ESP_ECDSA_CURVE_MAX) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    status = esp_ecdsa_validate_sha_alg(alg, curve);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if ((curve == ESP_ECDSA_CURVE_SECP192R1 && hash_length != ECDSA_SHA_LEN) ||
        (curve == ESP_ECDSA_CURVE_SECP256R1 && hash_length != ECDSA_SHA_LEN)
#if SOC_ECDSA_SUPPORT_CURVE_P384
        || (curve == ESP_ECDSA_CURVE_SECP384R1 && hash_length != ECDSA_SHA_LEN_P384)
#endif
    ) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (signature_length != 2 * key_len) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    status = check_ecdsa_signature_range(signature, key_len, curve);
    if (status != PSA_SUCCESS) {
        return status;
    }

    const uint8_t *public_key_buffer = NULL;
    size_t public_key_buffer_size = 0;
    uint8_t public_key[2 * MAX_ECDSA_COMPONENT_LEN + 1];

    /* The contents of key_buffer may either be the private key len bytes
    * (private key format), or 0x04 followed by the public key len bytes (public
    * key format). To ensure the key is in the latter format, the public key
    * is exported. */
    if (!PSA_KEY_TYPE_IS_PUBLIC_KEY(psa_get_key_type(attributes))) {
        // Private key is provided, convert it to public key
        size_t public_key_size = sizeof(public_key);
        size_t public_key_length = 0;

        status = psa_driver_wrapper_export_public_key(
            attributes,
            key_buffer,
            key_buffer_size,
            public_key,
            public_key_size,
            &public_key_length);
        if (status != PSA_SUCCESS) {
            return status;
        }
        public_key_buffer = public_key;
        public_key_buffer_size = public_key_length;
    } else {
        public_key_buffer = key_buffer;
        public_key_buffer_size = key_buffer_size;
    }

    if (public_key_buffer[0] != ECDSA_UNCOMPRESSED_POINT_FORMAT) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    // As PSA supports only uncompressed point format, the public key buffer size should be 2 * key_len + 1
    if (public_key_buffer_size != 2 * key_len + 1) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Verify the public key */
    ecc_point_t point;
    memset(&point, 0, sizeof(ecc_point_t));

    esp_ecdsa_change_endianness(public_key_buffer + 1, point.x, key_len);
    esp_ecdsa_change_endianness(public_key_buffer + 1 + key_len, point.y, key_len);
    point.len = key_len;

    /* Reject the identity (point at infinity, all-zero coords) explicitly —
     * esp_ecc_point_verify may not catch it on all peripherals. */
    bool qx_zero = true, qy_zero = true;
    for (size_t i = 0; i < key_len; i++) {
        qx_zero = qx_zero && (point.x[i] == 0);
        qy_zero = qy_zero && (point.y[i] == 0);
    }
    if (qx_zero && qy_zero) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (!esp_ecc_point_verify(&point)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    memset(operation, 0, sizeof(esp_ecdsa_transparent_verify_hash_operation_t));
    operation->curve = curve;
    operation->key_len = key_len;
    operation->sha_len = hash_length;
    esp_ecdsa_change_endianness(hash, operation->sha, key_len);
    esp_ecdsa_change_endianness(signature, operation->r, key_len);
    esp_ecdsa_change_endianness(signature + key_len, operation->s, key_len);
    /* The public key buffer is in the format 0x04 followed by the 2*key_len bytes public key */
    /* The first byte is the format byte, which is ECDSA_UNCOMPRESSED_POINT_FORMAT */
    /* The next key_len bytes are the x coordinate */
    /* The next key_len bytes are the y coordinate */
    esp_ecdsa_change_endianness(public_key_buffer + 1, operation->qx, key_len);
    esp_ecdsa_change_endianness(public_key_buffer + 1 + key_len, operation->qy, key_len);

    return PSA_SUCCESS;
}

psa_status_t esp_ecdsa_transparent_verify_hash_complete(esp_ecdsa_transparent_verify_hash_operation_t *operation)
{
    if (!operation) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    esp_ecdsa_acquire_hardware();

    ecdsa_hal_config_t conf = {
        .mode = ECDSA_MODE_SIGN_VERIFY,
        .curve = operation->curve,
        .sha_mode = ECDSA_Z_USER_PROVIDED,
    };

    int ret = ecdsa_hal_verify_signature(&conf, operation->sha, operation->r, operation->s, operation->qx, operation->qy, operation->key_len);

    esp_ecdsa_release_hardware();

    if (ret != 0) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    ESP_FAULT_ASSERT(ret == 0);

    return PSA_SUCCESS;
}

psa_status_t esp_ecdsa_transparent_verify_hash_abort(esp_ecdsa_transparent_verify_hash_operation_t *operation)
{
    if (operation) {
        mbedtls_platform_zeroize(operation, sizeof(esp_ecdsa_transparent_verify_hash_operation_t));
    }
    return PSA_SUCCESS;
}

psa_status_t esp_ecdsa_transparent_verify_hash(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length,
    const uint8_t *signature,
    size_t signature_length)
{
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;

    esp_ecdsa_transparent_verify_hash_operation_t operation;

    status = esp_ecdsa_transparent_verify_hash_start(&operation, attributes, key_buffer, key_buffer_size, alg, hash, hash_length, signature, signature_length);
    if (status == PSA_SUCCESS) {
        status = esp_ecdsa_transparent_verify_hash_complete(&operation);
    }

    esp_ecdsa_transparent_verify_hash_abort(&operation);
    return status;
}

#endif /* ESP_ECDSA_VERIFY_DRIVER_ENABLED */
