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
#include "psa/crypto.h"
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

    memcpy(sign_r, sign.signature, sign_r_len);
    memcpy(sign_s, sign.signature + sign_r_len, sign_s_len);

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

    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&key_attributes, 256);
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_SIGN | PSA_KEY_USAGE_VERIFY);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDSA);
    psa_status_t status = psa_generate_key(&key_attributes, &keypair->key_id);
    if (status != PSA_SUCCESS) {
        return ESP_FAIL;
    }

    size_t pub_key_len = 0;
    uint8_t pub_key[2 * SECP256R1_ECDSA_KEY_LEN + 1] = {0};
    status = psa_export_public_key(keypair->key_id, pub_key, sizeof(pub_key), &pub_key_len);
    if (status != PSA_SUCCESS) {
        return ESP_FAIL;
    }

    if (pub_key_len != sizeof(pub_key)) {
        return ESP_ERR_INVALID_SIZE;
    }

    memcpy(keypair->pub_key_x, pub_key + 1, SECP256R1_ECDSA_KEY_LEN);
    memcpy(keypair->pub_key_y, pub_key + 1 + SECP256R1_ECDSA_KEY_LEN, SECP256R1_ECDSA_KEY_LEN);

    psa_reset_key_attributes(&key_attributes);

    keypair->curve = 0;
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

    size_t signature_len = 0;
    uint8_t signature[sign_r_len + sign_s_len];
    psa_status_t status = psa_sign_hash(keypair->key_id, PSA_ALG_ECDSA(PSA_ALG_SHA_256), digest, len, signature, sign_r_len + sign_s_len, &signature_len);
    if (status != PSA_SUCCESS) {
        return ESP_FAIL;
    }

    if (signature_len != sign_r_len + sign_s_len) {
        return ESP_ERR_INVALID_SIZE;
    }

    memcpy(sign_r, signature, sign_r_len);
    memcpy(sign_s, signature + sign_r_len, sign_s_len);

    return ESP_OK;
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

    size_t pubkey_hexstr_size = sizeof(keypair->pub_key_x) * 2 + ECDSA_PUBKEY_PREFIX_SZ + 1;
    *pubkey_hexstr = calloc(pubkey_hexstr_size, sizeof(char));
    if (*pubkey_hexstr == NULL) {
        return ESP_ERR_NO_MEM;
    }

    char *pubkey_prefix = (keypair->pub_key_y[SECP256R1_ECDSA_KEY_LEN - 1] & 1) ? ECDSA_COMPRESSED_KEY_ODD_PREFIX : ECDSA_COMPRESSED_KEY_EVEN_PREFIX;
    memcpy(*pubkey_hexstr, pubkey_prefix, ECDSA_PUBKEY_PREFIX_SZ);

    int err = esp_att_utils_hexbuf_to_hexstr(keypair->pub_key_x, sizeof(keypair->pub_key_x), *pubkey_hexstr + ECDSA_PUBKEY_PREFIX_SZ, pubkey_hexstr_size - ECDSA_PUBKEY_PREFIX_SZ);
    if (err != ESP_OK) {
        free(*pubkey_hexstr);
        *pubkey_hexstr = NULL;
        return err;
    }

    return ESP_OK;
}

esp_err_t esp_att_utils_ecdsa_get_pubkey_digest(const esp_att_ecdsa_keypair_t *keypair, uint8_t *digest, const size_t len)
{
    if (keypair == NULL || digest == NULL || len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t pubkey_c[SECP256R1_ECDSA_KEY_LEN * 2] = {0};
    memcpy(pubkey_c, keypair->pub_key_x, sizeof(keypair->pub_key_x));
    memcpy(pubkey_c + SECP256R1_ECDSA_KEY_LEN, keypair->pub_key_y, sizeof(keypair->pub_key_y));

    uint8_t pubkey_digest[SHA256_DIGEST_SZ];
    psa_hash_operation_t hash_op = PSA_HASH_OPERATION_INIT;
    psa_status_t status = psa_hash_setup(&hash_op, PSA_ALG_SHA_256);
    if (status != PSA_SUCCESS) {
        return ESP_FAIL;
    }

    status = psa_hash_update(&hash_op, pubkey_c, sizeof(pubkey_c));
    if (status != PSA_SUCCESS) {
        return ESP_FAIL;
    }
    size_t pubkey_digest_len = 0;
    status = psa_hash_finish(&hash_op, pubkey_digest, len, &pubkey_digest_len);
    if (status != PSA_SUCCESS) {
        return ESP_FAIL;
    }

    if (pubkey_digest_len != len) {
        return ESP_ERR_INVALID_SIZE;
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
