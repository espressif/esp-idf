/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <stdint.h>
#include <string.h>
#include "psa/crypto.h"
#include "psa_crypto_driver_esp_hmac_opaque.h"
#include "esp_efuse.h"
#include "esp_efuse_chip.h"
#include "hal/hmac_types.h"
#include "esp_hmac.h"

static bool validate_hmac_opaque_key_attributes(const esp_hmac_opaque_key_t *opaque_key)
{
    // efuse_block is uint8_t, so it's always >= 0 (EFUSE_BLK0)
    if (opaque_key->efuse_block < EFUSE_BLK_MAX && esp_efuse_get_key_purpose(opaque_key->efuse_block) == ESP_EFUSE_KEY_PURPOSE_HMAC_UP) {
        return true;
    }
    return false;
}

psa_status_t esp_hmac_import_key_opaque(
    const psa_key_attributes_t *attributes,
    const uint8_t *data,
    size_t data_length,
    uint8_t *key_buffer,
    size_t key_buffer_size,
    size_t *key_buffer_length,
    size_t *bits)
{
    if (!attributes || !data || !key_buffer || !key_buffer_length || !bits) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_buffer_size < sizeof(esp_hmac_opaque_key_t) || data_length < sizeof(esp_hmac_opaque_key_t)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    const esp_hmac_opaque_key_t *opaque_key = (const esp_hmac_opaque_key_t *) data;
    bool valid = validate_hmac_opaque_key_attributes(opaque_key);
    if (!valid) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    memcpy(key_buffer, opaque_key, sizeof(esp_hmac_opaque_key_t));
    *key_buffer_length = sizeof(esp_hmac_opaque_key_t);
    *bits = psa_get_key_bits(attributes);
    return PSA_SUCCESS;
}

psa_status_t esp_hmac_abort_opaque(esp_hmac_opaque_operation_t *esp_hmac_ctx)
{
    if (!esp_hmac_ctx) {
        mbedtls_platform_zeroize(esp_hmac_ctx, sizeof(esp_hmac_opaque_operation_t));
    }
    return PSA_SUCCESS;
}

psa_status_t esp_hmac_setup_opaque(
    esp_hmac_opaque_operation_t *esp_hmac_ctx,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg)
{
    if (!esp_hmac_ctx || !attributes || !key_buffer || !key_buffer_size || !alg) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_buffer_size < sizeof(esp_hmac_opaque_key_t)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (!PSA_ALG_IS_HMAC(alg)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_algorithm_t hash_alg = PSA_ALG_GET_HASH(alg);

    if (hash_alg != PSA_ALG_SHA_256) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    memset(esp_hmac_ctx, 0, sizeof(esp_hmac_opaque_operation_t));

    const esp_hmac_opaque_key_t *opaque_key = (const esp_hmac_opaque_key_t *) key_buffer;

    if (!validate_hmac_opaque_key_attributes(opaque_key)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    esp_hmac_ctx->opaque_key = opaque_key;

    return PSA_SUCCESS;
}

static hmac_key_id_t translate_efuse_block_to_hmac_key_id(uint8_t efuse_block)
{
    return (hmac_key_id_t) (efuse_block - EFUSE_BLK_KEY0);
}

psa_status_t esp_hmac_update_opaque(esp_hmac_opaque_operation_t *esp_hmac_ctx, const uint8_t *data, size_t data_length)
{
    if (!esp_hmac_ctx || !data || data_length == 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    hmac_key_id_t hmac_key_id = HMAC_KEY_MAX;

#if SOC_KEY_MANAGER_HMAC_KEY_DEPLOY
    if (esp_hmac_ctx->opaque_key->use_km_key) {
        hmac_key_id = HMAC_KEY_KM;
    } else
#endif /* SOC_KEY_MANAGER_HMAC_KEY_DEPLOY */
    {
        hmac_key_id = translate_efuse_block_to_hmac_key_id(esp_hmac_ctx->opaque_key->efuse_block);
    }

    esp_err_t hmac_ret = esp_hmac_calculate(hmac_key_id, data, data_length, esp_hmac_ctx->hmac);
    if (hmac_ret == ESP_ERR_INVALID_ARG) {
        return PSA_ERROR_INVALID_ARGUMENT;
    } else if (hmac_ret == ESP_FAIL) {
        return PSA_ERROR_HARDWARE_FAILURE;
    } else if (hmac_ret == ESP_OK) {
        return PSA_SUCCESS;
    }

    return PSA_ERROR_CORRUPTION_DETECTED;
}

psa_status_t esp_hmac_finish_opaque(
    esp_hmac_opaque_operation_t *esp_hmac_ctx,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length)
{
    if (!esp_hmac_ctx || !mac || !mac_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (mac_size < ESP_HMAC_RESULT_SIZE) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    memcpy(mac, esp_hmac_ctx->hmac, ESP_HMAC_RESULT_SIZE);
    *mac_length = ESP_HMAC_RESULT_SIZE;

    return PSA_SUCCESS;
}

psa_status_t esp_hmac_compute_opaque(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length)
{

    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    esp_hmac_opaque_operation_t esp_hmac_ctx = {0};

    status = esp_hmac_setup_opaque(&esp_hmac_ctx, attributes, key_buffer, key_buffer_size, alg);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = esp_hmac_update_opaque(&esp_hmac_ctx, input, input_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = esp_hmac_finish_opaque(&esp_hmac_ctx, mac, mac_size, mac_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return PSA_SUCCESS;
}

psa_status_t esp_hmac_verify_finish_opaque(
    esp_hmac_opaque_operation_t *esp_hmac_ctx,
    const uint8_t *mac,
    size_t mac_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    uint8_t actual_mac[ESP_HMAC_RESULT_SIZE];
    size_t actual_mac_length = 0;

    status = esp_hmac_finish_opaque(esp_hmac_ctx, actual_mac, sizeof(actual_mac), &actual_mac_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if (memcmp(mac, actual_mac, mac_length) != 0) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    return PSA_SUCCESS;
}

/**
 * @brief Get the size of the ESP HMAC opaque key
 *
 * @param key_type    Key type
 * @param key_bits    Key bits
 * @return size_t     Size of the ESP HMAC opaque key
 */
size_t esp_hmac_opaque_size_function(
    psa_key_type_t key_type,
    size_t key_bits)
{
    (void)key_type;
    (void)key_bits;

    // Opaque keys always use the same size structure
    return sizeof(esp_hmac_opaque_key_t);
}
