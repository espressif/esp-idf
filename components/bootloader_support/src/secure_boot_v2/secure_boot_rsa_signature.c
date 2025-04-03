/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_log.h"
#include "esp_secure_boot.h"
#include "psa/crypto.h"

#include "secure_boot_signature_priv.h"

ESP_LOG_ATTR_TAG(TAG, "secure_boot_v2_rsa");

esp_err_t verify_rsa_signature_block(const ets_secure_boot_signature_t *sig_block, const uint8_t *image_digest, const ets_secure_boot_sig_block_t *trusted_block)
{
    if (!sig_block || !image_digest || !trusted_block) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;
    psa_status_t status;
    const unsigned rsa_key_size = sizeof(sig_block->block[0].signature);
    unsigned char *sig_be = calloc(1, rsa_key_size);

    if (sig_be == NULL) {
        return ESP_ERR_NO_MEM;
    }

    /* Create key attributes for RSA public key */
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;

    /* Set key attributes */
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_RSA_PSS(PSA_ALG_SHA_256));
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_RSA_PUBLIC_KEY);

    /* Set the key size in bits (RSA key size is typically 2048 or 3072 bits) */
    psa_set_key_bits(&key_attributes, PSA_BYTES_TO_BITS(rsa_key_size));

    /* Prepare the RSA public key in the format expected by PSA */
    /* PSA expects the key in big-endian format as a sequence of {N, E} */
    size_t n_size = rsa_key_size; /* N size in bytes */
    size_t e_size = sizeof(trusted_block->key.e); /* E size in bytes */
    size_t key_data_size = n_size + e_size + 8; /* Additional bytes for encoding */

    uint8_t *key_data = calloc(1, key_data_size);
    if (key_data == NULL) {
        free(sig_be);
        return ESP_ERR_NO_MEM;
    }

    /* Construct the key data - would normally need proper DER encoding,
       but PSA may accept raw concatenated N and E values */
    uint8_t *p = key_data;

    /* Copy N (modulus) */
    memcpy(p, trusted_block->key.n, n_size);
    p += n_size;

    /* Copy E (exponent) */
    memcpy(p, &trusted_block->key.e, e_size);

    /* Import the RSA public key */
    status = psa_import_key(&key_attributes, key_data, key_data_size, &key_id);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to import RSA public key, err: %d", status);
        ret = ESP_FAIL;
        goto cleanup;
    }

    /* Signature needs to be byte swapped into BE representation */
    for (int j = 0; j < rsa_key_size; j++) {
        sig_be[rsa_key_size - j - 1] = trusted_block->signature[j];
    }

    /* Verify the signature using PSA APIs */
    status = psa_verify_hash(key_id, PSA_ALG_RSA_PSS(PSA_ALG_SHA_256),
                            image_digest, ESP_SECURE_BOOT_DIGEST_LEN,
                            sig_be, rsa_key_size);

    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Signature verification failed, err: %d", status);
        ret = ESP_FAIL;
    } else {
        ESP_LOGI(TAG, "Signature verified successfully!");
        ret = ESP_OK;
    }

cleanup:
    /* Clean up resources */
    if (key_id != 0) {
        psa_destroy_key(key_id);
    }
    psa_reset_key_attributes(&key_attributes);
    free(key_data);
    free(sig_be);

    return ret;
}
