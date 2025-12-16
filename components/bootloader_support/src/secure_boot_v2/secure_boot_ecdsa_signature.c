/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_log.h"
#include "esp_secure_boot.h"
#include "rom/ecdsa.h"
#include "sdkconfig.h"
#include "psa/crypto.h"

#include "secure_boot_signature_priv.h"

ESP_LOG_ATTR_TAG(TAG, "secure_boot_v2_ecdsa");

#if CONFIG_SECURE_BOOT_ECDSA_KEY_LEN_384_BITS
#define ECDSA_INTEGER_LEN 48
#else
#define ECDSA_INTEGER_LEN 32
#endif /* CONFIG_SECURE_BOOT_ECDSA_KEY_LEN_384_BITS */

esp_err_t verify_ecdsa_signature_block(const ets_secure_boot_signature_t *sig_block, const uint8_t *image_digest, const ets_secure_boot_sig_block_t *trusted_block)
{
    if (!sig_block || !image_digest || !trusted_block) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;
    psa_status_t status;

    /* Prepare public key for verification */
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_handle = 0;

    /* Set key attributes according to the curve */
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_VERIFY_HASH);

    uint8_t key_size = 0;
    psa_ecc_family_t curve_family;

    switch(trusted_block->ecdsa.key.curve_id) {
        case ECDSA_CURVE_P192:
            key_size = 24;
            curve_family = PSA_ECC_FAMILY_SECP_R1;
            psa_set_key_bits(&key_attributes, PSA_BYTES_TO_BITS(key_size));
            break;
        case ECDSA_CURVE_P256:
            key_size = 32;
            curve_family = PSA_ECC_FAMILY_SECP_R1;
            psa_set_key_bits(&key_attributes, PSA_BYTES_TO_BITS(key_size));
            break;
#if CONFIG_SECURE_BOOT_ECDSA_KEY_LEN_384_BITS
        case ECDSA_CURVE_P384:
            key_size = 48;
            curve_family = PSA_ECC_FAMILY_SECP_R1;
            psa_set_key_bits(&key_attributes, PSA_BYTES_TO_BITS(key_size));
            break;
#endif /* CONFIG_SECURE_BOOT_ECDSA_KEY_LEN_384_BITS */
        default:
            ESP_LOGE(TAG, "Invalid curve ID");
            return ESP_ERR_INVALID_ARG;
    }

    psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(curve_family));

    /* Prepare the public key data from X and Y coordinates */
    uint8_t public_key[(2 * ECDSA_INTEGER_LEN) + 1];
    uint8_t x_point[ECDSA_INTEGER_LEN] = {0};
    uint8_t y_point[ECDSA_INTEGER_LEN] = {0};

    /* Convert key points from little-endian to big-endian format */
    for (int i = 0; i < key_size; i++) {
        x_point[i] = trusted_block->ecdsa.key.point[key_size - i - 1];

        y_point[i] = trusted_block->ecdsa.key.point[2 * key_size - i - 1];
    }

    public_key[0] = 0x04; /* Uncompressed point format */

    /* Combine X and Y into a single public key buffer */
    memcpy(public_key + 1, x_point, key_size);
    memcpy(public_key + 1 + key_size, y_point, key_size);

    /* Import the public key */
    status = psa_import_key(&key_attributes, public_key, (2 * key_size) + 1, &key_handle);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to import key, err:%d", status);
        ret = ESP_FAIL;
        goto cleanup;
    }

    /* Convert signature from little-endian to big-endian format */
    uint8_t signature[2 * ECDSA_INTEGER_LEN] = {0};
    for (int i = 0; i < key_size; i++) {
        signature[i] = trusted_block->ecdsa.signature[key_size - i - 1];
        signature[key_size + i] = trusted_block->ecdsa.signature[2 * key_size - i - 1];
    }

    /* Verify the signature */
    status = psa_verify_hash(key_handle, PSA_ALG_ECDSA(PSA_ALG_SHA_256),
                             image_digest, ESP_SECURE_BOOT_DIGEST_LEN,
                             signature, 2 * key_size);

    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Signature verification failed, err:%d", status);
        ret = ESP_FAIL;
    }

cleanup:
    /* Clean up resources */
    if (key_handle) {
        psa_destroy_key(key_handle);
    }
    psa_reset_key_attributes(&key_attributes);

    return ret;
}
