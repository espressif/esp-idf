/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "esp_log.h"
#include "esp_err.h"

#if ESP_TEE_BUILD
#include "bootloader_sha.h"
#include "esp_tee_sec_storage.h"
#endif

#include "esp_random.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/sha256.h"

#include "esp_attestation_utils.h"

#define ECDSA_PUBKEY_PREFIX_SZ  (0x02)

#define ECDSA_COMPRESSED_KEY_EVEN_PREFIX  ("02")
#define ECDSA_COMPRESSED_KEY_ODD_PREFIX   ("03")

/* Forward declaration */
static esp_err_t gen_ecdsa_keypair_secp256r1(esp_att_ecdsa_keypair_t *keypair);
static esp_err_t get_ecdsa_sign_secp256r1(const esp_att_ecdsa_keypair_t *keypair, const uint8_t *digest, const size_t len,
                                          uint8_t *sign_r, size_t sign_r_len, uint8_t *sign_s, size_t sign_s_len);

static const char *TAG = "esp_att_utils_crypto";

#if ESP_TEE_BUILD

static esp_err_t gen_ecdsa_keypair_secp256r1(esp_att_ecdsa_keypair_t *keypair)
{
    if (keypair == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    memset(keypair, 0x00, sizeof(esp_att_ecdsa_keypair_t));

    esp_tee_sec_storage_key_cfg_t key_cfg = {
        .id = (const char *)(ESP_ATT_TK_KEY_ID),
        .type = ESP_SEC_STG_KEY_ECDSA_SECP256R1,
    };

    esp_err_t err = esp_tee_sec_storage_gen_key(&key_cfg);
    if (err == ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "Using pre-existing key...");
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to generate ECDSA keypair (%d)", err);
        return err;
    }

    esp_tee_sec_storage_ecdsa_pubkey_t pubkey = {};
    err = esp_tee_sec_storage_ecdsa_get_pubkey(&key_cfg, &pubkey);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to fetch ECDSA pubkey (%d)", err);
        return err;
    }

    memcpy(keypair->pub_key_x, pubkey.pub_x, sizeof(pubkey.pub_x));
    memcpy(keypair->pub_key_y, pubkey.pub_y, sizeof(pubkey.pub_y));

    return ESP_OK;
}

static esp_err_t get_ecdsa_sign_secp256r1(const esp_att_ecdsa_keypair_t *keypair, const uint8_t *digest, const size_t len,
                                          uint8_t *sign_r, size_t sign_r_len, uint8_t *sign_s, size_t sign_s_len)
{
    if (sign_r == NULL || sign_s == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (sign_r_len == 0 || sign_s_len == 0) {
        return ESP_ERR_INVALID_SIZE;
    }

    esp_tee_sec_storage_key_cfg_t key_cfg = {
        .id = (const char *)(ESP_ATT_TK_KEY_ID),
        .type = ESP_SEC_STG_KEY_ECDSA_SECP256R1,
    };

    esp_tee_sec_storage_ecdsa_sign_t sign = {};
    esp_err_t err = esp_tee_sec_storage_ecdsa_sign(&key_cfg, (uint8_t *)digest, len, &sign);
    if (err != ESP_OK) {
        return err;
    }

    memcpy(sign_r, sign.sign_r, sign_r_len);
    memcpy(sign_s, sign.sign_s, sign_s_len);

    return ESP_OK;
}

#else

static int rng_func(void *rng_ctx, unsigned char *output, size_t len)
{
    esp_fill_random(output, len);
    return 0;
}

static esp_err_t gen_ecdsa_keypair_secp256r1(esp_att_ecdsa_keypair_t *keypair)
{
    if (keypair == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(keypair, 0x00, sizeof(esp_att_ecdsa_keypair_t));

    int ret = -1;
    esp_err_t err = ESP_FAIL;

    mbedtls_ecdsa_context ecdsa_ctx;
    mbedtls_ecdsa_init(&ecdsa_ctx);

    ret = mbedtls_ecdsa_genkey(&ecdsa_ctx, MBEDTLS_ECP_DP_SECP256R1, rng_func, NULL);
    if (ret != 0) {
        goto exit;
    }

    size_t pvt_len = mbedtls_mpi_size(&ecdsa_ctx.MBEDTLS_PRIVATE(d));
    ret = mbedtls_mpi_write_binary(&ecdsa_ctx.MBEDTLS_PRIVATE(d), (unsigned char *)keypair->pvt_key, pvt_len);
    if (ret != 0) {
        goto exit;
    }

    size_t pubx_len = mbedtls_mpi_size(&(ecdsa_ctx.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(X)));
    ret = mbedtls_mpi_write_binary(&(ecdsa_ctx.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(X)), (unsigned char *)(keypair->pub_key_x), pubx_len);
    if (ret != 0) {
        goto exit;
    }

    size_t puby_len = mbedtls_mpi_size(&(ecdsa_ctx.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Y)));
    ret = mbedtls_mpi_write_binary(&(ecdsa_ctx.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Y)), (unsigned char *)(keypair->pub_key_y), puby_len);
    if (ret != 0) {
        goto exit;
    }

    keypair->curve = 0;
    err = ESP_OK;

exit:
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to generate ECDSA keypair (-0x%X)", -ret);
    }
    mbedtls_ecdsa_free(&ecdsa_ctx);
    return err;
}

static esp_err_t get_ecdsa_sign_secp256r1(const esp_att_ecdsa_keypair_t *keypair, const uint8_t *digest, const size_t len,
                                          uint8_t *sign_r, size_t sign_r_len, uint8_t *sign_s, size_t sign_s_len)
{
    if (sign_r == NULL || sign_s == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (sign_r_len == 0 || sign_s_len == 0) {
        return ESP_ERR_INVALID_SIZE;
    }

    esp_err_t err = ESP_FAIL;

    mbedtls_ecp_keypair pvt_key;
    mbedtls_mpi r, s;

    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);
    mbedtls_ecp_keypair_init(&pvt_key);

    int ret = mbedtls_ecp_read_key(MBEDTLS_ECP_DP_SECP256R1, &pvt_key, keypair->pvt_key, sizeof(keypair->pvt_key));
    if (ret != 0) {
        goto exit;
    }

    mbedtls_ecdsa_context ecdsa_ctx;
    mbedtls_ecdsa_init(&ecdsa_ctx);

    ret = mbedtls_ecdsa_from_keypair(&ecdsa_ctx, &pvt_key);
    if (ret != 0) {
        goto exit;
    }

    ret = mbedtls_ecdsa_sign(&ecdsa_ctx.MBEDTLS_PRIVATE(grp), &r, &s, &ecdsa_ctx.MBEDTLS_PRIVATE(d),
                             digest, len, rng_func, NULL);
    if (ret != 0) {
        return ret;
    }

    size_t r_len = mbedtls_mpi_size(&r);
    if (r_len > sign_s_len) {
        goto exit;
    }

    ret = mbedtls_mpi_write_binary(&r, (unsigned char *)(sign_r), r_len);
    if (ret != 0) {
        goto exit;
    }

    size_t s_len = mbedtls_mpi_size(&s);
    if (s_len > sign_s_len) {
        goto exit;
    }

    ret = mbedtls_mpi_write_binary(&s, (unsigned char *)(sign_s), s_len);
    if (ret != 0) {
        goto exit;
    }

    err = ESP_OK;

exit:
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to generate ECDSA signature (-0x%X)", -ret);
    }

    mbedtls_ecdsa_free(&ecdsa_ctx);
    mbedtls_ecp_keypair_free(&pvt_key);
    mbedtls_mpi_free(&s);
    mbedtls_mpi_free(&r);

    return err;
}

#endif

/* TODO: The public key generated here needs to be authorized with the relying party */
esp_err_t esp_att_utils_ecdsa_gen_keypair_secp256r1(esp_att_ecdsa_keypair_t *keypair)
{
    return gen_ecdsa_keypair_secp256r1(keypair);
}

esp_err_t esp_att_utils_ecdsa_get_pubkey(const esp_att_ecdsa_keypair_t *keypair, char **pubkey_hexstr)
{
    if (keypair == NULL || pubkey_hexstr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = ESP_FAIL;

    size_t hexstr_len = sizeof(keypair->pub_key_x) * 2 + ECDSA_PUBKEY_PREFIX_SZ + 1;
    char *hexstr = calloc(hexstr_len, sizeof(uint8_t));
    if (hexstr == NULL) {
        err = ESP_ERR_NO_MEM;
        goto exit;
    }

    /* Checking the parity of the y-component of the public key */
    char *pubkey_prefix = (keypair->pub_key_y[SECP256R1_ECDSA_KEY_LEN - 1] & 1)
                          ? ECDSA_COMPRESSED_KEY_ODD_PREFIX
                          : ECDSA_COMPRESSED_KEY_EVEN_PREFIX;
    memcpy(hexstr, pubkey_prefix, ECDSA_PUBKEY_PREFIX_SZ);

    err = esp_att_utils_hexbuf_to_hexstr(keypair->pub_key_x, sizeof(keypair->pub_key_x),
                                         &hexstr[ECDSA_PUBKEY_PREFIX_SZ], hexstr_len - ECDSA_PUBKEY_PREFIX_SZ);
    if (err != ESP_OK) {
        goto exit;
    }

    *pubkey_hexstr = hexstr;
    return ESP_OK;

exit:
    free(hexstr);
    return err;
}

esp_err_t esp_att_utils_ecdsa_get_pubkey_digest(const esp_att_ecdsa_keypair_t *keypair, uint8_t *digest, const size_t len)
{
    if (keypair == NULL || digest == NULL || len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t pubkey_c[SECP256R1_ECDSA_KEY_LEN * 2] = {0};
    memcpy(pubkey_c, keypair->pub_key_x, SECP256R1_ECDSA_KEY_LEN);
    memcpy(pubkey_c + SECP256R1_ECDSA_KEY_LEN, keypair->pub_key_y, SECP256R1_ECDSA_KEY_LEN);

    uint8_t pubkey_digest[SHA256_DIGEST_SZ];
    int ret = mbedtls_sha256((const unsigned char *)pubkey_c, sizeof(pubkey_c), pubkey_digest, false);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to calculate pubkey digest (-%X)", -ret);
        return ESP_FAIL;
    }

    memcpy(digest, pubkey_digest, len);
    return ESP_OK;
}

esp_err_t esp_att_utils_ecdsa_get_sign(const esp_att_ecdsa_keypair_t *keypair, const uint8_t *digest, const size_t len,
                                       char **sign_r_hexstr, char **sign_s_hexstr)
{
    if (keypair == NULL || digest == NULL || sign_r_hexstr == NULL || sign_s_hexstr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (len == 0) {
        return ESP_ERR_INVALID_SIZE;
    }

    esp_err_t err = ESP_FAIL;

    unsigned char sign_r[SECP256R1_ECDSA_KEY_LEN] = {0}, sign_s[SECP256R1_ECDSA_KEY_LEN] = {0};
    err = get_ecdsa_sign_secp256r1(keypair, digest, len, sign_r, sizeof(sign_r), sign_s, sizeof(sign_s));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to generate ECDSA signature");
        goto exit;
    }

    size_t sign_hexstr_len = SECP256R1_ECDSA_KEY_LEN * 2 + 1;

    *sign_r_hexstr = calloc(sign_hexstr_len, sizeof(char));
    if (*sign_r_hexstr == NULL) {
        err = ESP_ERR_NO_MEM;
        goto exit;
    }

    err = esp_att_utils_hexbuf_to_hexstr(sign_r, sizeof(sign_r), *sign_r_hexstr, sign_hexstr_len);
    if (err != ESP_OK) {
        goto exit;
    }

    *sign_s_hexstr = calloc(sign_hexstr_len, sizeof(char));
    if (*sign_s_hexstr == NULL) {
        free(*sign_r_hexstr);
        *sign_r_hexstr = NULL;
        err = ESP_ERR_NO_MEM;
        goto exit;
    }

    err = esp_att_utils_hexbuf_to_hexstr(sign_s, sizeof(sign_s), *sign_s_hexstr, sign_hexstr_len);
    if (err != ESP_OK) {
        goto exit;
    }

    err = ESP_OK;

exit:
    return err;
}
