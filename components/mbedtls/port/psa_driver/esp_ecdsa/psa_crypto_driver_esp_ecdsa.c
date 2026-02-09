/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * ESP ECDSA PSA Crypto Opaque Driver
 *
 * This driver provides PSA Crypto API integration for ESP ECDSA hardware.
 */

#include <string.h>
#include "soc/soc_caps.h"
#include "esp_log.h"

#include "esp_crypto_lock.h"
#include "esp_crypto_periph_clk.h"

#include "esp_efuse.h"
#include "esp_efuse_chip.h"

#if SOC_ECDSA_SUPPORTED
#include "hal/ecdsa_types.h"
#include "hal/ecdsa_hal.h"
#include "hal/ecdsa_ll.h"
#endif /* SOC_ECDSA_SUPPORTED */

#if SOC_ECC_SUPPORTED
#include "hal/ecc_ll.h"
#include "ecc_impl.h"
#endif /* SOC_ECC_SUPPORTED */

#if SOC_MPI_SUPPORTED && SOC_ECDSA_USES_MPI
#include "hal/mpi_ll.h"
#endif /* SOC_MPI_SUPPORTED && SOC_ECDSA_USES_MPI */

#include "psa_crypto_driver_esp_ecdsa.h"
#include "psa_crypto_driver_wrappers_no_static.h"
#include "sdkconfig.h"

static const char *TAG = "psa_crypto_driver_esp_ecdsa";

#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
#include "esp_tee_sec_storage.h"
#endif /* CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN */

/* Key lengths for different ECDSA curves */
#define ECDSA_KEY_LEN_P192          24
#define ECDSA_KEY_LEN_P256          32
#define ECDSA_KEY_LEN_P384          48

#define ECDSA_UNCOMPRESSED_POINT_FORMAT 0x04

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_CONSTANT_TIME_CM
#include "esp_timer.h"
#include "soc/chip_revision.h"
#include "hal/efuse_hal.h"

#if CONFIG_ESP_CRYPTO_DPA_PROTECTION_LEVEL_HIGH
/*
 * This is the maximum time (in us) required for performing 1 ECDSA signature
 * in this configuration along some additional margin considerations
 */
#define ECDSA_MAX_SIG_TIME 24000
#else /* CONFIG_ESP_CRYPTO_DPA_PROTECTION_LEVEL_HIGH */
#define ECDSA_MAX_SIG_TIME  17500
#endif /* !CONFIG_ESP_CRYPTO_DPA_PROTECTION_LEVEL_HIGH */

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_MASKING_CM
#define DUMMY_OP_COUNT ECDSA_SIGN_MAX_DUMMY_OP_COUNT
#else /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_MASKING_CM */
#define DUMMY_OP_COUNT 0
#endif /* !CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_MASKING_CM */
#define ECDSA_CM_FIXED_SIG_TIME ECDSA_MAX_SIG_TIME * (DUMMY_OP_COUNT + 1)

#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_CONSTANT_TIME_CM */

// Helper function to get curve from mbedtls group ID
static esp_ecdsa_curve_t psa_bits_to_ecdsa_curve(size_t key_len)
{
    switch (key_len) {
        case ECDSA_KEY_LEN_P192:
            return ESP_ECDSA_CURVE_SECP192R1;
        case ECDSA_KEY_LEN_P256:
            return ESP_ECDSA_CURVE_SECP256R1;
#if SOC_ECDSA_SUPPORT_CURVE_P384
        case ECDSA_KEY_LEN_P384:
            return ESP_ECDSA_CURVE_SECP384R1;
#endif
        default:
            return ESP_ECDSA_CURVE_MAX;
    }
}

#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
static esp_tee_sec_storage_type_t esp_ecdsa_curve_to_tee_sec_storage_type(esp_ecdsa_curve_t curve)
{
    switch (curve) {
        case ESP_ECDSA_CURVE_SECP256R1:
            return ESP_SEC_STG_KEY_ECDSA_SECP256R1;
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP384R1_SIGN
        case ESP_ECDSA_CURVE_SECP384R1:
            return ESP_SEC_STG_KEY_ECDSA_SECP384R1;
#endif
        default:
            return (esp_tee_sec_storage_type_t)-1;
        }
}
#endif /* CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN */

static void change_endianess(const uint8_t *old_point, uint8_t *new_point, uint8_t len)
{
    /* When the size is less than MAX_ECDSA_COMPONENT_LEN, it should be padded with 0 bytes*/
    memset(new_point, 0x0, len);

    for(int i = 0; i < len; i++) {
        new_point[i] = old_point[len - i - 1];
    }
}

static psa_status_t validate_ecdsa_sha_alg(psa_algorithm_t alg, const esp_ecdsa_curve_t curve)
{
    if ((PSA_ALG_SIGN_GET_HASH(alg) != PSA_ALG_SHA_256 && (curve == ESP_ECDSA_CURVE_SECP192R1 || curve == ESP_ECDSA_CURVE_SECP256R1))
#if SOC_ECDSA_SUPPORT_CURVE_P384
        || (PSA_ALG_SIGN_GET_HASH(alg) != PSA_ALG_SHA_384 && curve == ESP_ECDSA_CURVE_SECP384R1)
#endif
    ) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    return PSA_SUCCESS;
}

#if SOC_ECDSA_SUPPORTED
static void esp_ecdsa_acquire_hardware(void)
{
    esp_crypto_ecdsa_lock_acquire();

    esp_crypto_ecdsa_enable_periph_clk(true);

    esp_crypto_ecc_enable_periph_clk(true);

#if SOC_ECDSA_USES_MPI
    if (ecdsa_ll_is_mpi_required()) {
    /* We need to reset the MPI peripheral because ECDSA peripheral
     * of some targets use the MPI peripheral as well.
     */
        esp_crypto_mpi_enable_periph_clk(true);
    }
#endif /* SOC_ECDSA_USES_MPI */
}

static void esp_ecdsa_release_hardware(void)
{
    esp_crypto_ecdsa_enable_periph_clk(false);

    esp_crypto_ecc_enable_periph_clk(false);

#if SOC_ECDSA_USES_MPI
    if (ecdsa_ll_is_mpi_required()) {
        esp_crypto_mpi_enable_periph_clk(false);
    }
#endif /* SOC_ECDSA_USES_MPI */

    esp_crypto_ecdsa_lock_release();
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

    size_t key_len = PSA_BITS_TO_BYTES(psa_get_key_bits(attributes));
    esp_ecdsa_curve_t curve = psa_bits_to_ecdsa_curve(key_len);
    if (curve == ESP_ECDSA_CURVE_MAX) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    status = validate_ecdsa_sha_alg(alg, curve);
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

    change_endianess(public_key_buffer + 1, point.x, key_len);
    change_endianess(public_key_buffer + 1 + key_len, point.y, key_len);
    point.len = key_len;

    if (!esp_ecc_point_verify(&point)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    memset(operation, 0, sizeof(esp_ecdsa_transparent_verify_hash_operation_t));
    operation->curve = curve;
    operation->key_len = key_len;
    operation->sha_len = hash_length;
    change_endianess(hash, operation->sha, key_len);
    change_endianess(signature, operation->r, key_len);
    change_endianess(signature + key_len, operation->s, key_len);
    /* The public key buffer is in the format 0x04 followed by the 2*key_len bytes public key */
    /* The first byte is the format byte, which is ECDSA_UNCOMPRESSED_POINT_FORMAT */
    /* The next key_len bytes are the x coordinate */
    /* The next key_len bytes are the y coordinate */
    change_endianess(public_key_buffer + 1, operation->qx, key_len);
    change_endianess(public_key_buffer + 1 + key_len, operation->qy, key_len);

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

    return PSA_SUCCESS;
}

psa_status_t esp_ecdsa_transparent_verify_hash_abort(esp_ecdsa_transparent_verify_hash_operation_t *operation)
{
    if (operation) {
        memset(operation, 0, sizeof(esp_ecdsa_transparent_verify_hash_operation_t));
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
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = esp_ecdsa_transparent_verify_hash_complete(&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return esp_ecdsa_transparent_verify_hash_abort(&operation);
}
#endif /* SOC_ECDSA_SUPPORTED */

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
#if SOC_ECDSA_SUPPORT_CURVE_P384
/**
 * @brief Check if the extracted efuse blocks are valid
 *
 * @param high_blk High efuse block number
 * @param low_blk Low efuse block number
 * @return true if both blocks are valid, false otherwise
 */
static bool is_efuse_blk_valid(int high_blk, int low_blk)
{
    return (high_blk >= EFUSE_BLK0 && high_blk < EFUSE_BLK_MAX &&
            low_blk >= EFUSE_BLK0 && low_blk < EFUSE_BLK_MAX);
}
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */

/**
 * @brief Validate if the efuse block(s) have the appropriate ECDSA key purpose for the given curve
 *
 * This function validates that the provided efuse block(s) have been programmed with the appropriate
 * ECDSA key purpose for the specified curve type. It handles both curve-specific key purposes
 * (when SOC_ECDSA_SUPPORT_CURVE_SPECIFIC_KEY_PURPOSES is defined) and generic ECDSA key purpose.
 *
 * For SECP384R1 curve, it checks for both high and low key blocks when supported.
 * For SECP192R1 and SECP256R1 curves, it validates the single block.
 *
 * @param[in] curve The ECDSA curve to validate the key purpose for
 * @param[in] efuse_blk The efuse block(s) to validate (can be combined for 384-bit keys)
 *
 * @return
 *      - 0 on success (block(s) have correct purpose)
 *      - PSA_ERROR_INVALID_ARGUMENT if input parameters are invalid
 *      - PSA_ERROR_NOT_PERMITTED if block(s) don't have appropriate key purpose
 */
static psa_status_t esp_ecdsa_validate_efuse_block(esp_ecdsa_curve_t curve, int efuse_blk)
{
    int low_blk = efuse_blk;
    esp_efuse_purpose_t expected_key_purpose_low;
#if SOC_ECDSA_SUPPORT_CURVE_SPECIFIC_KEY_PURPOSES && ((!defined(CONFIG_IDF_TARGET_ESP32P4) && SOC_EFUSE_ECDSA_KEY_P192) || EFUSE_LL_HAS_ECDSA_KEY_P192)
#if SOC_ECDSA_SUPPORT_CURVE_P384
    int high_blk;
    HAL_ECDSA_EXTRACT_KEY_BLOCKS(efuse_blk, high_blk, low_blk);
    if (!is_efuse_blk_valid(high_blk, low_blk)) {
        ESP_LOGE(TAG, "Invalid efuse block selected");
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    esp_efuse_purpose_t expected_key_purpose_high;
#endif

    switch (curve) {
        case ESP_ECDSA_CURVE_SECP192R1:
            expected_key_purpose_low = ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY_P192;
            break;
        case ESP_ECDSA_CURVE_SECP256R1:
            expected_key_purpose_low = ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY_P256;
            break;
#if SOC_ECDSA_SUPPORT_CURVE_P384
        case ESP_ECDSA_CURVE_SECP384R1:
            expected_key_purpose_low = ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY_P384_L;
            expected_key_purpose_high = ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY_P384_H;
            break;
#endif
        default:
            ESP_LOGE(TAG, "Unsupported ECDSA curve: %ld", curve);
            return PSA_ERROR_INVALID_ARGUMENT;
    }
#else /* SOC_ECDSA_SUPPORT_CURVE_SPECIFIC_KEY_PURPOSES && ((!defined(CONFIG_IDF_TARGET_ESP32P4) && SOC_EFUSE_ECDSA_KEY_P192) || EFUSE_LL_HAS_ECDSA_KEY_P192) */
    expected_key_purpose_low = ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY;
#endif  /* !SOC_ECDSA_SUPPORT_CURVE_SPECIFIC_KEY_PURPOSES */

    if (expected_key_purpose_low != esp_efuse_get_key_purpose((esp_efuse_block_t)low_blk)) {
        ESP_LOGE(TAG, "Key burned in efuse has incorrect purpose, expected: %d, got: %d, low_blk: %d", expected_key_purpose_low, esp_efuse_get_key_purpose((esp_efuse_block_t)low_blk), low_blk);
        return PSA_ERROR_NOT_PERMITTED;
    }

#if SOC_ECDSA_SUPPORT_CURVE_SPECIFIC_KEY_PURPOSES && ((!defined(CONFIG_IDF_TARGET_ESP32P4) && SOC_EFUSE_ECDSA_KEY_P384) || EFUSE_LL_HAS_ECDSA_KEY_P384)
#if SOC_ECDSA_SUPPORT_CURVE_P384
    // Only check high block purpose for P384 curves that actually use it
    if (curve == ESP_ECDSA_CURVE_SECP384R1 &&
        expected_key_purpose_high != esp_efuse_get_key_purpose((esp_efuse_block_t)high_blk)) {
        ESP_LOGE(TAG, "Key burned in efuse has incorrect purpose for high block");
        return PSA_ERROR_NOT_PERMITTED;
    }
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */
#endif /* SOC_ECDSA_SUPPORT_CURVE_SPECIFIC_KEY_PURPOSES && ((!defined(CONFIG_IDF_TARGET_ESP32P4) && SOC_EFUSE_ECDSA_KEY_P384) || EFUSE_LL_HAS_ECDSA_KEY_P384) */

    return PSA_SUCCESS;
}
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN */

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN || CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
static psa_status_t validate_ecdsa_opaque_key_attributes(const psa_key_attributes_t *attributes, const esp_ecdsa_opaque_key_t *opaque_key)
{
    esp_ecdsa_curve_t expected_curve = psa_bits_to_ecdsa_curve(PSA_BITS_TO_BYTES(psa_get_key_bits(attributes)));

    if (expected_curve != opaque_key->curve) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
    if (!(opaque_key->use_km_key
#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
        || opaque_key->tee_key_id
#endif
    )) {
        psa_status_t status = esp_ecdsa_validate_efuse_block(opaque_key->curve, opaque_key->efuse_block);
        if (status != PSA_SUCCESS) {
            return status;
        }
    }
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN */
    return PSA_SUCCESS;
}

psa_status_t esp_ecdsa_opaque_import_key(
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

    if (key_buffer_size < sizeof(esp_ecdsa_opaque_key_t) || data_length < sizeof(esp_ecdsa_opaque_key_t)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    const esp_ecdsa_opaque_key_t *opaque_key = (const esp_ecdsa_opaque_key_t *) data;
    psa_status_t status = validate_ecdsa_opaque_key_attributes(attributes, opaque_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

    memcpy(key_buffer, opaque_key, sizeof(esp_ecdsa_opaque_key_t));
    *key_buffer_length = sizeof(esp_ecdsa_opaque_key_t);
    *bits = psa_get_key_bits(attributes);
    return PSA_SUCCESS;
}

psa_status_t esp_ecdsa_opaque_sign_hash_start(
    esp_ecdsa_opaque_sign_hash_operation_t *operation,
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

    if (key_buffer_size < sizeof(esp_ecdsa_opaque_key_t)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    // Check if ECDSA algorithm
    if (!PSA_ALG_IS_ECDSA(alg)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    esp_ecdsa_opaque_key_t *opaque_key = (esp_ecdsa_opaque_key_t *) key_buffer;
    psa_status_t status = validate_ecdsa_opaque_key_attributes(attributes, opaque_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = validate_ecdsa_sha_alg(alg, opaque_key->curve);
    if (status != PSA_SUCCESS) {
        return status;
    }

    size_t component_len = PSA_BITS_TO_BYTES(psa_get_key_bits(attributes));

    // Validate hash length
    if ((opaque_key->curve == ESP_ECDSA_CURVE_SECP192R1 && hash_length != ECDSA_SHA_LEN
#if SOC_ECDSA_SUPPORTED
            && esp_efuse_is_ecdsa_p192_curve_supported()
#endif
        ) || (opaque_key->curve == ESP_ECDSA_CURVE_SECP256R1 && hash_length != ECDSA_SHA_LEN
#if SOC_ECDSA_SUPPORTED
            && esp_efuse_is_ecdsa_p256_curve_supported()
#endif
        )
#if SOC_ECDSA_SUPPORT_CURVE_P384
        || (opaque_key->curve == ESP_ECDSA_CURVE_SECP384R1 && hash_length != ECDSA_SHA_LEN_P384)
#endif
    ) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    memset(operation, 0, sizeof(esp_ecdsa_opaque_sign_hash_operation_t));

    operation->key_len = component_len;

#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
    if (opaque_key->tee_key_id) {
        // The ESP-TEE API expects the hash in big endian order
        memcpy(operation->sha, hash, component_len);
    } else
#endif /* CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN */
    {
        change_endianess(hash, operation->sha, component_len);
    }

    operation->opaque_key = opaque_key;
    operation->alg = alg;
    operation->sha_len = hash_length;

    return PSA_SUCCESS;
}

psa_status_t esp_ecdsa_opaque_sign_hash_complete(
    esp_ecdsa_opaque_sign_hash_operation_t *operation,
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

#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
    esp_ecdsa_opaque_key_t *opaque_key = operation->opaque_key;
    if (opaque_key->tee_key_id) {
        esp_tee_sec_storage_type_t tee_sec_storage_type = esp_ecdsa_curve_to_tee_sec_storage_type(opaque_key->curve);
        if (tee_sec_storage_type == (esp_tee_sec_storage_type_t) -1) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }

        esp_tee_sec_storage_key_cfg_t cfg = {
            .id = opaque_key->tee_key_id,
            .type = tee_sec_storage_type
        };

        esp_tee_sec_storage_ecdsa_sign_t sign = {};
        esp_err_t err = esp_tee_sec_storage_ecdsa_sign(&cfg, operation->sha, operation->sha_len, &sign);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to get signature: 0x%08lx", err);
            return PSA_ERROR_INVALID_ARGUMENT;
        }

        memcpy(signature, sign.signature, 2 * component_len);
        *signature_length = 2 * component_len;

    } else
#endif /* CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN */
    {
#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
        // Check if the ECDSA peripheral is supported on this chip revision
        if (!ecdsa_ll_is_supported()) {
            ESP_LOGE(TAG, "ECDSA peripheral not supported on this chip revision");
            return PSA_ERROR_NOT_SUPPORTED;
        }

        ecdsa_sign_type_t k_type = ECDSA_K_TYPE_TRNG;

#if CONFIG_MBEDTLS_ECDSA_DETERMINISTIC && SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
        if (PSA_ALG_ECDSA_IS_DETERMINISTIC(operation->alg)) {
            if (ecdsa_ll_is_deterministic_mode_supported()) {
                k_type = ECDSA_K_TYPE_DETERMINISITIC;
            } else {
                return PSA_ERROR_NOT_SUPPORTED;
            }
        }
#endif /* CONFIG_MBEDTLS_ECDSA_DETERMINISTIC && SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE */

        uint8_t zeroes[MAX_ECDSA_COMPONENT_LEN] = {0};

        // Acquire hardware
        esp_ecdsa_acquire_hardware();

        bool process_again = false;

#if CONFIG_MBEDTLS_ECDSA_DETERMINISTIC && !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP
        uint16_t deterministic_loop_number __attribute__((unused)) = 1;
#endif /* CONFIG_MBEDTLS_ECDSA_DETERMINISTIC && !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP */

        do {
            ecdsa_hal_config_t conf = {
                .mode = ECDSA_MODE_SIGN_GEN,
                .curve = operation->opaque_key->curve,
                .sha_mode = ECDSA_Z_USER_PROVIDED,
                .sign_type = k_type,
            };

#if CONFIG_MBEDTLS_ECDSA_DETERMINISTIC && !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP
            if (k_type == ECDSA_K_TYPE_DETERMINISITIC) {
                conf.loop_number = deterministic_loop_number++;
            }
#endif /* CONFIG_MBEDTLS_ECDSA_DETERMINISTIC && !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP */

            // Set key source (consistent with esp_ecdsa_pk_conf_t)
            if (operation->opaque_key->use_km_key) {
                conf.use_km_key = 1;
            } else {
                conf.efuse_key_blk = operation->opaque_key->efuse_block;
            }

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_CONSTANT_TIME_CM
            uint64_t sig_time = esp_timer_get_time();
#endif
            // Generate signature
            ecdsa_hal_gen_signature(&conf, operation->sha, operation->r, operation->s, component_len);

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_CONSTANT_TIME_CM
            if (!ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 102)) {
                sig_time = esp_timer_get_time() - sig_time;
                if (sig_time < ECDSA_CM_FIXED_SIG_TIME) {
                    esp_rom_delay_us(ECDSA_CM_FIXED_SIG_TIME - sig_time);
                }
            }
#endif
            // Check if we need to retry (zero signature or operation failed)
            process_again = !ecdsa_hal_get_operation_result()
                            || !memcmp(operation->r, zeroes, component_len)
                            || !memcmp(operation->s, zeroes, component_len);

#if CONFIG_MBEDTLS_ECDSA_DETERMINISTIC && SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE && !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP
            if (k_type == ECDSA_K_TYPE_DETERMINISITIC) {
                process_again |= !ecdsa_hal_det_signature_k_check();
            }
#endif /* CONFIG_MBEDTLS_ECDSA_DETERMINISTIC && SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE && !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP */
        } while (process_again);

        esp_ecdsa_release_hardware();

        // Convert r from little-endian to big-endian and copy to output
        change_endianess(operation->r, signature, component_len);
        // Convert s from little-endian to big-endian and copy to output
        change_endianess(operation->s, signature + component_len, component_len);
#else
        // This is an invalid operation as the hardware ECDSA signing is not supported on this chip
        // and still the key is opaque.
        return PSA_ERROR_NOT_SUPPORTED;
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN */
    }

    *signature_length = 2 * component_len;

    return PSA_SUCCESS;
}

psa_status_t esp_ecdsa_opaque_sign_hash_abort(esp_ecdsa_opaque_sign_hash_operation_t *operation)
{
    if (operation) {
        memset(operation, 0, sizeof(esp_ecdsa_opaque_sign_hash_operation_t));
    }
    return PSA_SUCCESS;
}


psa_status_t esp_ecdsa_opaque_sign_hash(
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
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;

    esp_ecdsa_opaque_sign_hash_operation_t operation;

    status = esp_ecdsa_opaque_sign_hash_start(&operation, attributes, key_buffer, key_buffer_size, alg, hash, hash_length);
    if (status != PSA_SUCCESS) {
        esp_ecdsa_opaque_sign_hash_abort(&operation);
        return status;
    }

    status = esp_ecdsa_opaque_sign_hash_complete(&operation, signature, signature_size, signature_length);
    if (status != PSA_SUCCESS) {
        esp_ecdsa_opaque_sign_hash_abort(&operation);
        return status;
    }

    return esp_ecdsa_opaque_sign_hash_abort(&operation);
}

psa_status_t esp_ecdsa_opaque_export_public_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    uint8_t *data,
    size_t data_size,
    size_t *data_length)
{
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;

    if (!attributes || !key_buffer || !data || !data_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_buffer_size < sizeof(esp_ecdsa_opaque_key_t)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    const esp_ecdsa_opaque_key_t *opaque_key = (const esp_ecdsa_opaque_key_t *) key_buffer;
    status = validate_ecdsa_opaque_key_attributes(attributes, opaque_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

    // Get curve parameters from attributes
    size_t key_len = PSA_BITS_TO_BYTES(psa_get_key_bits(attributes));

    // As the ESP ECDSA driver only supports uncompressed point format,
    // the public key is exported in uncompressed point format (ECDSA_UNCOMPRESSED_POINT_FORMAT || x || y)
    // Need 1 byte for format + key_len bytes for x + key_len bytes for y
    size_t required_size = 1 + (2 * key_len);
    if (data_size < required_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
    if (opaque_key->tee_key_id) {
        esp_tee_sec_storage_type_t tee_sec_storage_type = esp_ecdsa_curve_to_tee_sec_storage_type(opaque_key->curve);
        if (tee_sec_storage_type == (esp_tee_sec_storage_type_t) -1) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }

        esp_tee_sec_storage_key_cfg_t cfg = {
            .id = opaque_key->tee_key_id,
            .type = tee_sec_storage_type,
        };

        esp_tee_sec_storage_ecdsa_pubkey_t pubkey;
        if (esp_tee_sec_storage_ecdsa_get_pubkey(&cfg, &pubkey) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to get public key from secure storage");
            return PSA_ERROR_GENERIC_ERROR;
        }

        data[0] = ECDSA_UNCOMPRESSED_POINT_FORMAT;
        memcpy(data + 1, pubkey.pub_x, key_len);
        memcpy(data + 1 + key_len, pubkey.pub_y, key_len);
    }
    else
#endif /* CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN */
    {
#if SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
        // Check if the ECDSA peripheral is supported on this chip revision
        if (!ecdsa_ll_is_supported()) {
            ESP_LOGE(TAG, "ECDSA peripheral not supported on this chip revision");
            return PSA_ERROR_NOT_SUPPORTED;
        }

        uint8_t zeroes[MAX_ECDSA_COMPONENT_LEN] = {0};
        uint8_t qx[MAX_ECDSA_COMPONENT_LEN];
        uint8_t qy[MAX_ECDSA_COMPONENT_LEN];

        ecdsa_hal_config_t conf = {
            .mode = ECDSA_MODE_EXPORT_PUBKEY,
            .curve = opaque_key->curve,
        };

        if (opaque_key->use_km_key) {
            conf.use_km_key = 1;
        } else {
            conf.efuse_key_blk = opaque_key->efuse_block;
        }

        esp_ecdsa_acquire_hardware();
        bool process_again = false;

        do {
            ecdsa_hal_export_pubkey(&conf, qx, qy, key_len);

            process_again = !ecdsa_hal_get_operation_result()
                            || !memcmp(qx, zeroes, key_len)
                            || !memcmp(qy, zeroes, key_len);

        } while (process_again);

        esp_ecdsa_release_hardware();

        // Format: uncompressed point (ECDSA_UNCOMPRESSED_POINT_FORMAT byte followed by x and y coordinates)
        data[0] = ECDSA_UNCOMPRESSED_POINT_FORMAT;

        // Convert qx from little-endian to big-endian and copy to output
        change_endianess(qx, data + 1, key_len);

        // Convert qy from little-endian to big-endian and copy to output
        change_endianess(qy, data + 1 + key_len, key_len);
#else
        // This is an invalid operation as the export public key is not supported on this chip
        // and still the key is opaque
        return PSA_ERROR_NOT_SUPPORTED;
#endif /* SOC_ECDSA_SUPPORT_EXPORT_PUBKEY */
    }

    *data_length = required_size;

    return PSA_SUCCESS;
}

size_t esp_ecdsa_opaque_size_function(
    psa_key_type_t key_type,
    size_t key_bits)
{
    (void)key_type;
    (void)key_bits;

    // Opaque keys always use the same size structure
    return sizeof(esp_ecdsa_opaque_key_t);
}
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN */
