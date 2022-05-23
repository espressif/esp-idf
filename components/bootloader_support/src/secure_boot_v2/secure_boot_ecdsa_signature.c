/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
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
#include "mbedtls/ecp.h"
#include "rom/ecdsa.h"

#include "secure_boot_signature_priv.h"

static const char *TAG = "secure_boot_v2_ecdsa";

#define ECDSA_INTEGER_LEN 32

esp_err_t verify_ecdsa_signature_block(const ets_secure_boot_signature_t *sig_block, const uint8_t *image_digest, const ets_secure_boot_sig_block_t *trusted_block)
{
    if (!sig_block || !image_digest || !trusted_block) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret;

    mbedtls_mpi r, s;

    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    /* Initialise ECDSA context */
    mbedtls_ecdsa_context ecdsa_context;
    mbedtls_ecdsa_init(&ecdsa_context);

    uint8_t key_size = 0;

    switch(trusted_block->ecdsa.key.curve_id) {
        case ECDSA_CURVE_P192:
            key_size = 24;
            mbedtls_ecp_group_load(&ecdsa_context.MBEDTLS_PRIVATE(grp), MBEDTLS_ECP_DP_SECP192R1);
            break;
        case ECDSA_CURVE_P256:
            key_size = 32;
            mbedtls_ecp_group_load(&ecdsa_context.MBEDTLS_PRIVATE(grp), MBEDTLS_ECP_DP_SECP256R1);
            break;
        default:
            ESP_LOGE(TAG, "Invalid curve ID");
            return ESP_ERR_INVALID_ARG;
    }

    uint8_t x_point[ECDSA_INTEGER_LEN] = {};
    uint8_t y_point[ECDSA_INTEGER_LEN] = {};
    uint8_t _r[ECDSA_INTEGER_LEN] = {};
    uint8_t _s[ECDSA_INTEGER_LEN] = {};

    /* Convert r and s components to big endian format */
    for (int i = 0; i < key_size; i++) {
        _r[i] = trusted_block->ecdsa.signature[key_size - i - 1];
        _s[i] = trusted_block->ecdsa.signature[2 * key_size - i - 1];
    }

    /* Extract r and s components from RAW ECDSA signature of 64 bytes */
    ret = mbedtls_mpi_read_binary(&r, _r, key_size);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed mbedtls_mpi_read_binary(r), err:%d", ret);
        mbedtls_ecdsa_free(&ecdsa_context);
        return ESP_FAIL;
    }

    ret = mbedtls_mpi_read_binary(&s, _s, key_size);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed mbedtls_mpi_read_binary(s), err:%d", ret);
        mbedtls_mpi_free(&r);
        mbedtls_ecdsa_free(&ecdsa_context);
        return ESP_FAIL;
    }

    size_t plen = mbedtls_mpi_size(&ecdsa_context.MBEDTLS_PRIVATE(grp).P);

    for (int i = 0; i < plen; i++) {
        x_point[i] = trusted_block->ecdsa.key.point[plen - 1 - i];
        y_point[i] = trusted_block->ecdsa.key.point[2 * plen - 1 - i];
    }

    /* Extract X and Y components from ECDSA public key */
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&ecdsa_context.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(X), x_point, plen));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&ecdsa_context.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Y), y_point, plen));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&ecdsa_context.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Z), 1));

    ret = mbedtls_ecdsa_verify(&ecdsa_context.MBEDTLS_PRIVATE(grp), image_digest, ESP_SECURE_BOOT_DIGEST_LEN, &ecdsa_context.MBEDTLS_PRIVATE(Q), &r, &s);

cleanup:
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    mbedtls_ecdsa_free(&ecdsa_context);
    return ret;
}
