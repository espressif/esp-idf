/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_log.h"
#include "esp_secure_boot.h"
#include "mbedtls/sha256.h"
#include "mbedtls/x509.h"
#include "mbedtls/md.h"
#include "mbedtls/platform.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

#include "secure_boot_signature_priv.h"

static const char *TAG = "secure_boot_v2_rsa";

esp_err_t verify_rsa_signature_block(const ets_secure_boot_signature_t *sig_block, const uint8_t *image_digest, const ets_secure_boot_sig_block_t *trusted_block)
{
    if (!sig_block || !image_digest || !trusted_block) {
        return ESP_ERR_INVALID_ARG;
    }

    int ret = 0;
    mbedtls_rsa_context pk;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const unsigned rsa_key_size = sizeof(sig_block->block[0].signature);
    unsigned char *sig_be = calloc(1, rsa_key_size);
    if (sig_be == NULL) {
        return ESP_ERR_NO_MEM;
    }
    unsigned char *buf = calloc(1, rsa_key_size);
    if (buf == NULL) {
        free(sig_be);
        return ESP_ERR_NO_MEM;
    }

    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0);
    if (ret != 0) {
        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned -0x%04x", ret);
        goto exit_outer;
    }

    const mbedtls_mpi N = { .MBEDTLS_PRIVATE(s) = 1,
        .MBEDTLS_PRIVATE(n) = sizeof(trusted_block->key.n)/sizeof(mbedtls_mpi_uint),
        .MBEDTLS_PRIVATE(p) = (void *)trusted_block->key.n,
    };
    const mbedtls_mpi e = { .MBEDTLS_PRIVATE(s) = 1,
        .MBEDTLS_PRIVATE(n) = sizeof(trusted_block->key.e)/sizeof(mbedtls_mpi_uint), // 1
        .MBEDTLS_PRIVATE(p) = (void *)&trusted_block->key.e,
    };
    mbedtls_rsa_init(&pk);
    mbedtls_rsa_set_padding(&pk,MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256);
    ret = mbedtls_rsa_import(&pk, &N, NULL, NULL, NULL, &e);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed mbedtls_rsa_import, err: %d", ret);
        goto exit_inner;
    }

    ret = mbedtls_rsa_complete(&pk);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed mbedtls_rsa_complete, err: %d", ret);
        goto exit_inner;
    }

    ret = mbedtls_rsa_check_pubkey(&pk);
    if (ret != 0) {
        ESP_LOGI(TAG, "Key is not an RSA key -%0x", -ret);
        goto exit_inner;
    }

    /* Signature needs to be byte swapped into BE representation */
    for (int j = 0; j < rsa_key_size; j++) {
        sig_be[rsa_key_size - j - 1] = trusted_block->signature[j];
    }

    ret = mbedtls_rsa_public( &pk, sig_be, buf);
    if (ret != 0) {
        ESP_LOGE(TAG, "mbedtls_rsa_public failed, err: %d", ret);
        goto exit_inner;
    }

    ret = mbedtls_rsa_rsassa_pss_verify( &pk, MBEDTLS_MD_SHA256, ESP_SECURE_BOOT_DIGEST_LEN, image_digest, sig_be);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed mbedtls_rsa_rsassa_pss_verify, err: %d", ret);
    } else {
        ESP_LOGI(TAG, "Signature verified successfully!");
    }
exit_inner:
    mbedtls_rsa_free(&pk);
exit_outer:
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    free(sig_be);
    free(buf);
    return ret;
}
