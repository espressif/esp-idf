/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_log.h"
#include "esp_secure_boot.h"
#include "psa/crypto.h"
#include "mbedtls/pk.h"
#include "mbedtls/rsa.h"

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


    /* Prepare the RSA public key data */
    const mbedtls_mpi N = { .MBEDTLS_PRIVATE(s) = 1,
        .MBEDTLS_PRIVATE(n) = sizeof(trusted_block->key.n)/sizeof(mbedtls_mpi_uint),
        .MBEDTLS_PRIVATE(p) = (void *)trusted_block->key.n,
    };
    const mbedtls_mpi e = { .MBEDTLS_PRIVATE(s) = 1,
        .MBEDTLS_PRIVATE(n) = sizeof(trusted_block->key.e)/sizeof(mbedtls_mpi_uint), // 1
        .MBEDTLS_PRIVATE(p) = (void *)&trusted_block->key.e,
    };

    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);

    mbedtls_pk_setup(&pk, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA));
    mbedtls_rsa_context *rsa = mbedtls_pk_rsa(pk);

    ret = mbedtls_rsa_import(rsa, &N, NULL, NULL, NULL, &e);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to import RSA public key, err: %d", ret);
        mbedtls_pk_free(&pk);
        goto cleanup;
    }
    ret = mbedtls_rsa_complete(rsa);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to complete RSA context, err: %d", ret);
        mbedtls_pk_free(&pk);
        goto cleanup;
    }

    // Load the public key into PSA
    ret = mbedtls_pk_get_psa_attributes(&pk, PSA_KEY_USAGE_VERIFY_HASH, &key_attributes);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to get key attributes, err: %d", ret);
        mbedtls_pk_free(&pk);
        goto cleanup;
    }

    /* Set key attributes */
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_RSA_PSS(PSA_ALG_SHA_256));
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_RSA_PUBLIC_KEY);

    ret = mbedtls_pk_import_into_psa(&pk, &key_attributes, &key_id);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to import key into PSA, err: %d", ret);
        mbedtls_pk_free(&pk);
        goto cleanup;
    }

    mbedtls_rsa_free(rsa);
    mbedtls_pk_free(&pk);

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
    free(sig_be);

    return ret;
}
