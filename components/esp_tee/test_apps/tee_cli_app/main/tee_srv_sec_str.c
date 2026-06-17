/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>

#include "soc/soc_caps.h"
#include "esp_event.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_console.h"
#include "argtable3/argtable3.h"

#include "psa/crypto.h"

#include "esp_tee_sec_storage.h"
#include "example_tee_srv.h"

#define SHA256_DIGEST_SZ         (32)
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP384R1_SIGN
#define MAX_ECDSA_KEY_LEN        (48)
#else
#define MAX_ECDSA_KEY_LEN        (32)
#endif /* CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP384R1_SIGN */

#define AES256_GCM_TAG_LEN       (16)
#define AES256_GCM_IV_LEN        (12)
#define MAX_AES_PLAINTEXT_LEN    (256)

#define ECDSA_SECP256R1_KEY_LEN  (32)

static const char *TAG = "tee_sec_stg";

static esp_err_t hexstr_to_hexbuf(const char *hexstr, size_t hexstr_len, void *hexbuf, size_t hexbuf_sz)
{
    if (hexstr == NULL || hexbuf == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (hexstr_len == 0 || hexbuf_sz < (hexstr_len / 2)) {
        return ESP_ERR_INVALID_SIZE;
    }

    const uint8_t *const_bytes = (const uint8_t *)hexstr;
    uint8_t *bytes = (uint8_t *)hexbuf;

    for (size_t i = 0; i < hexstr_len; i += 2) {
        uint8_t upper_nibble = (const_bytes[i] >= 'a') ? (const_bytes[i] - 'a' + 10) : (const_bytes[i] - '0');
        uint8_t lower_nibble = (const_bytes[i + 1] >= 'a') ? (const_bytes[i + 1] - 'a' + 10) : (const_bytes[i + 1] - '0');

        bytes[i / 2] = (upper_nibble << 4) | (lower_nibble);
    }

    return ESP_OK;
}

static esp_err_t hexbuf_to_hexstr(const void *hexbuf, size_t hexbuf_sz, char *hexstr, size_t hexstr_len)
{
    if (hexbuf == NULL || hexstr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (hexbuf_sz == 0 || hexstr_len < (hexbuf_sz * 2 + 1)) {
        return ESP_ERR_INVALID_SIZE;
    }

    const uint8_t *bytes = (const uint8_t *)hexbuf;

    for (size_t i = 0; i < hexbuf_sz; i++) {
        for (int shift = 0; shift < 2; shift++) {
            uint8_t nibble = (bytes[i] >> (shift ? 0 : 4)) & 0x0F;
            if (nibble < 10) {
                hexstr[i * 2 + shift] = '0' + nibble;
            } else {
                hexstr[i * 2 + shift] = 'a' + nibble - 10;
            }
        }
    }
    hexstr[hexbuf_sz * 2] = '\0';

    return ESP_OK;
}

static esp_err_t verify_ecdsa_secp256r1_sign(const uint8_t *digest, size_t len, const esp_tee_sec_storage_ecdsa_pubkey_t *pubkey, const esp_tee_sec_storage_ecdsa_sign_t *sign)
{
    if (pubkey == NULL || digest == NULL || sign == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (len == 0) {
        return ESP_ERR_INVALID_SIZE;
    }

    esp_err_t err = ESP_FAIL;

    psa_key_id_t key_id = 0;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));

    uint8_t pub_key[2 * ECDSA_SECP256R1_KEY_LEN + 1];
    pub_key[0] = 0x04;
    memcpy(pub_key + 1, pubkey->pub_x, ECDSA_SECP256R1_KEY_LEN);
    memcpy(pub_key + 1 + ECDSA_SECP256R1_KEY_LEN, pubkey->pub_y, ECDSA_SECP256R1_KEY_LEN);

    psa_status_t status = psa_import_key(&key_attributes, pub_key, sizeof(pub_key), &key_id);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    status = psa_verify_hash(key_id, PSA_ALG_ECDSA(PSA_ALG_SHA_256), digest, len, sign->signature, 2 * ECDSA_SECP256R1_KEY_LEN);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to verify hash: %ld", status);
        goto exit;
    }
    err = ESP_OK;

exit:
    if (key_id) {
        psa_destroy_key(key_id);
    }
    psa_reset_key_attributes(&key_attributes);
    return err;
}

static struct {
    struct arg_str *msg;
    struct arg_end *end;
} cmd_get_msg_sha256_args;

static int get_msg_sha256(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &cmd_get_msg_sha256_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, cmd_get_msg_sha256_args.end, argv[0]);
        return ESP_ERR_INVALID_ARG;
    }

    const char *msg = (const char *)cmd_get_msg_sha256_args.msg->sval[0];

    uint8_t msg_digest[SHA256_DIGEST_SZ];
    size_t msg_len = strlen(msg);
    size_t digest_len = 0;
    psa_status_t status = psa_hash_compute(PSA_ALG_SHA_256, (const uint8_t *)msg, msg_len, msg_digest, sizeof(msg_digest), &digest_len);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to calculate message hash!");
        return ESP_FAIL;
    }

    size_t digest_hexstr_len = SHA256_DIGEST_SZ * 2 + 1;
    char *digest_hexstr = calloc(digest_hexstr_len, sizeof(char));
    if (digest_hexstr == NULL) {
        return ESP_ERR_NO_MEM;
    }

    hexbuf_to_hexstr(msg_digest, sizeof(msg_digest), digest_hexstr, digest_hexstr_len);
    ESP_LOGI(TAG, "Message digest (SHA256) -\n%s", digest_hexstr);
    free(digest_hexstr);

    return ESP_OK;
}

void register_cmd_msg_sha256(void)
{
    cmd_get_msg_sha256_args.msg = arg_str1(NULL, NULL, "\"<msg>\"", "Message for SHA256 digest calculation");
    cmd_get_msg_sha256_args.end = arg_end(2);

    const esp_console_cmd_t cmd_get_msg_sha256 = {
        .command = "get_msg_sha256",
        .help = "Get the SHA256 digest for the given message",
        .hint = NULL,
        .func = &get_msg_sha256,
        .argtable = &cmd_get_msg_sha256_args,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd_get_msg_sha256));
}

static struct {
    struct arg_str *key_str_id;
    struct arg_int *key_type;
    struct arg_end *end;
} tee_sec_stg_gen_key_args;

static int tee_sec_stg_gen_key(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&tee_sec_stg_gen_key_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, tee_sec_stg_gen_key_args.end, argv[0]);
        return 1;
    }

    esp_err_t err = ESP_FAIL;

    esp_tee_sec_storage_key_cfg_t cfg = {
        .id = (const char *)tee_sec_stg_gen_key_args.key_str_id->sval[0],
        .type = (esp_tee_sec_storage_type_t)tee_sec_stg_gen_key_args.key_type->ival[0]
    };

    err = esp_tee_sec_storage_clear_key(cfg.id);
    if (err != ESP_OK && err != ESP_ERR_NOT_FOUND) {
        ESP_LOGE(TAG, "Failed to clear key %d!", cfg.id);
        goto exit;
    }

    err = esp_tee_sec_storage_gen_key(&cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to generate key!");
        goto exit;
    }

    ESP_LOGI(TAG, "Generated %s key with ID %s",
             (cfg.type == ESP_SEC_STG_KEY_ECDSA_SECP256R1) ? "ECDSA_SECP256R1" : "AES256",
             cfg.id);

exit:
    return err;
}

void register_srv_sec_stg_gen_key(void)
{
    tee_sec_stg_gen_key_args.key_str_id = arg_str1(NULL, NULL, "<key_id>", "TEE Secure storage key ID");
    tee_sec_stg_gen_key_args.key_type = arg_int1(NULL, NULL, "<key_type>", "Key type (0: AES256, 1: ECDSA_SECP256R1)");
    tee_sec_stg_gen_key_args.end = arg_end(2);

    const esp_console_cmd_t tee_sec_stg = {
        .command = "tee_sec_stg_gen_key",
        .help = "Generate and store a new key of the specified type with the given ID",
        .hint = NULL,
        .func = &tee_sec_stg_gen_key,
        .argtable = &tee_sec_stg_gen_key_args,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&tee_sec_stg));
}

static struct {
    struct arg_str *key_str_id;
    struct arg_str *msg_sha256;
    struct arg_end *end;
} tee_sec_stg_sign_args;

static int tee_sec_stg_sign(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &tee_sec_stg_sign_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, tee_sec_stg_sign_args.end, argv[0]);
        return ESP_ERR_INVALID_ARG;
    }

    const char *msg_sha256 = (const char *)tee_sec_stg_sign_args.msg_sha256->sval[0];
    size_t msg_sha256_len = strnlen(msg_sha256, SHA256_DIGEST_SZ * 2 + 1);
    if (msg_sha256_len != SHA256_DIGEST_SZ * 2) {
        ESP_LOGE(TAG, "Invalid input digest!");
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t digest[SHA256_DIGEST_SZ] = {};
    hexstr_to_hexbuf(msg_sha256, msg_sha256_len, digest, sizeof(digest));

    esp_tee_sec_storage_key_cfg_t cfg = {
        .id = (const char *)tee_sec_stg_sign_args.key_str_id->sval[0],
        .type = ESP_SEC_STG_KEY_ECDSA_SECP256R1
    };

    esp_tee_sec_storage_ecdsa_sign_t sign = {};
    esp_err_t err = esp_tee_sec_storage_ecdsa_sign(&cfg, digest, sizeof(digest), &sign);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to generate signature!");
        goto exit;
    }

    size_t sign_hexstr_len = (MAX_ECDSA_KEY_LEN * 2) * 2 + 1;
    char *sign_hexstr = calloc(sign_hexstr_len, sizeof(char));
    if (sign_hexstr == NULL) {
        err = ESP_ERR_NO_MEM;
        goto exit;
    }

    hexbuf_to_hexstr(&sign, sizeof(sign), sign_hexstr, sign_hexstr_len);
    ESP_LOGI(TAG, "Generated signature -\n%s", sign_hexstr);
    free(sign_hexstr);

    esp_tee_sec_storage_ecdsa_pubkey_t pubkey = {};
    err = esp_tee_sec_storage_ecdsa_get_pubkey(&cfg, &pubkey);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to fetch public-key!");
        goto exit;
    }

    size_t pubkey_hexstr_len = (MAX_ECDSA_KEY_LEN * 2) * 2 + 1;
    char *pubkey_hexstr = calloc(pubkey_hexstr_len, sizeof(char));
    if (pubkey_hexstr == NULL) {
        err = ESP_ERR_NO_MEM;
        goto exit;
    }

    hexbuf_to_hexstr(&pubkey, sizeof(pubkey), pubkey_hexstr, pubkey_hexstr_len);
    ESP_LOGI(TAG, "Public key (Uncompressed) -\n04%s", pubkey_hexstr);
    free(pubkey_hexstr);

    err = verify_ecdsa_secp256r1_sign(digest, sizeof(digest), &pubkey, &sign);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to verify signature!");
        goto exit;
    }

    ESP_LOGI(TAG, "Signature verified successfully!");

exit:
    return err;
}

void register_srv_sec_stg_sign(void)
{
    tee_sec_stg_sign_args.key_str_id = arg_str1(NULL, NULL, "<key_id>", "TEE Secure storage key ID");
    tee_sec_stg_sign_args.msg_sha256 = arg_str1(NULL, NULL, "<msg_sha256>", "SHA256 digest of the message to be signed and verified");
    tee_sec_stg_sign_args.end = arg_end(2);

    const esp_console_cmd_t tee_sec_stg = {
        .command = "tee_sec_stg_sign",
        .help = "Sign a message using the ECDSA keypair stored with the given key ID and verify the signature",
        .hint = NULL,
        .func = &tee_sec_stg_sign,
        .argtable = &tee_sec_stg_sign_args,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&tee_sec_stg));
}

static struct {
    struct arg_str *key_str_id;
    struct arg_str *plaintext;
    struct arg_end *end;
} tee_sec_stg_encrypt_args;

static int tee_sec_stg_encrypt(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&tee_sec_stg_encrypt_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, tee_sec_stg_encrypt_args.end, argv[0]);
        return 1;
    }

    esp_err_t err = ESP_FAIL;
    uint8_t tag[AES256_GCM_TAG_LEN];
    uint8_t iv[AES256_GCM_IV_LEN];
    const char *key_id = (const char *)tee_sec_stg_encrypt_args.key_str_id->sval[0];

    const char *plaintext = tee_sec_stg_encrypt_args.plaintext->sval[0];
    size_t plaintext_len = strnlen(plaintext, MAX_AES_PLAINTEXT_LEN);
    if (plaintext_len == MAX_AES_PLAINTEXT_LEN && plaintext[MAX_AES_PLAINTEXT_LEN] != '\0') {
        ESP_LOGE(TAG, "Plaintext too long (max - %d bytes)", MAX_AES_PLAINTEXT_LEN);
        err = ESP_ERR_INVALID_ARG;
        goto exit;
    }

    if (plaintext_len % 2 != 0) {
        ESP_LOGE(TAG, "Invalid plaintext - should be a hex string");
        err = ESP_ERR_INVALID_ARG;
        goto exit;
    }

    size_t plaintext_buf_len = plaintext_len / 2;
    uint8_t *plaintext_buf = calloc(plaintext_buf_len, sizeof(uint8_t));
    if (plaintext_buf == NULL) {
        err = ESP_ERR_NO_MEM;
        goto exit;
    }
    hexstr_to_hexbuf(plaintext, plaintext_len, plaintext_buf, plaintext_buf_len);

    uint8_t *ciphertext_buf = calloc(plaintext_buf_len, sizeof(uint8_t));
    if (ciphertext_buf == NULL) {
        err = ESP_ERR_NO_MEM;
        goto exit;
    }

    esp_tee_sec_storage_aead_ctx_t ctx = {
        .key_id = key_id,
        .input = (uint8_t *)plaintext_buf,
        .input_len = plaintext_buf_len
    };

    err = esp_tee_sec_storage_aead_encrypt(&ctx, iv, sizeof(iv), tag, sizeof(tag), ciphertext_buf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to encrypt data: %s", esp_err_to_name(err));
        goto exit;
    }

    char *ciphertext = calloc(plaintext_len + 1, sizeof(char));
    if (ciphertext == NULL) {
        err = ESP_ERR_NO_MEM;
        goto exit;
    }
    hexbuf_to_hexstr(ciphertext_buf, plaintext_buf_len, ciphertext, plaintext_len + 1);

    char tag_hexstr[AES256_GCM_TAG_LEN * 2 + 1];
    hexbuf_to_hexstr(tag, sizeof(tag), tag_hexstr, sizeof(tag_hexstr));

    char iv_hexstr[AES256_GCM_IV_LEN * 2 + 1];
    hexbuf_to_hexstr(iv, sizeof(iv), iv_hexstr, sizeof(iv_hexstr));

    ESP_LOGI(TAG, "Ciphertext -\n%s", ciphertext);
    ESP_LOGI(TAG, "IV -\n%s", iv_hexstr);
    ESP_LOGI(TAG, "Tag -\n%s", tag_hexstr);

    free(plaintext_buf);
    free(ciphertext_buf);
    free(ciphertext);

exit:
    return err;
}

void register_srv_sec_stg_encrypt(void)
{
    tee_sec_stg_encrypt_args.key_str_id = arg_str1(NULL, NULL, "<key_id>", "TEE Secure storage key ID");
    tee_sec_stg_encrypt_args.plaintext = arg_str1(NULL, NULL, "<plaintext>", "Plaintext to be encrypted");
    tee_sec_stg_encrypt_args.end = arg_end(2);

    const esp_console_cmd_t tee_sec_stg = {
        .command = "tee_sec_stg_encrypt",
        .help = "Encrypt data using AES-GCM key with the given ID from secure storage",
        .hint = NULL,
        .func = &tee_sec_stg_encrypt,
        .argtable = &tee_sec_stg_encrypt_args,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&tee_sec_stg));
}

static struct {
    struct arg_str *key_str_id;
    struct arg_str *ciphertext;
    struct arg_str *iv;
    struct arg_str *tag;
    struct arg_end *end;
} tee_sec_stg_decrypt_args;

static int tee_sec_stg_decrypt(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&tee_sec_stg_decrypt_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, tee_sec_stg_decrypt_args.end, argv[0]);
        return 1;
    }

    esp_err_t err = ESP_FAIL;
    const char *key_id = (const char *)tee_sec_stg_decrypt_args.key_str_id->sval[0];

    const char *iv_hexstr = tee_sec_stg_decrypt_args.iv->sval[0];
    uint8_t iv[AES256_GCM_IV_LEN];
    hexstr_to_hexbuf(iv_hexstr, strlen(iv_hexstr), iv, sizeof(iv));

    const char *tag_hexstr = tee_sec_stg_decrypt_args.tag->sval[0];
    uint8_t tag[AES256_GCM_TAG_LEN];
    hexstr_to_hexbuf(tag_hexstr, strlen(tag_hexstr), tag, sizeof(tag));

    const char *ciphertext = tee_sec_stg_decrypt_args.ciphertext->sval[0];
    size_t ciphertext_len = strnlen(ciphertext, MAX_AES_PLAINTEXT_LEN);
    if (ciphertext_len == MAX_AES_PLAINTEXT_LEN && ciphertext[MAX_AES_PLAINTEXT_LEN] != '\0') {
        ESP_LOGE(TAG, "Ciphertext too long (max - %d bytes)", MAX_AES_PLAINTEXT_LEN);
        err = ESP_ERR_INVALID_ARG;
        goto exit;
    }

    if (ciphertext_len % 2 != 0) {
        ESP_LOGE(TAG, "Invalid plaintext - should be a hex string");
        err = ESP_ERR_INVALID_ARG;
        goto exit;
    }

    size_t ciphertext_buf_len = ciphertext_len / 2;
    uint8_t *ciphertext_buf = calloc(ciphertext_buf_len, sizeof(uint8_t));
    if (ciphertext_buf == NULL) {
        err = ESP_ERR_NO_MEM;
        goto exit;
    }
    hexstr_to_hexbuf(ciphertext, ciphertext_len, ciphertext_buf, ciphertext_buf_len);

    uint8_t *plaintext_buf = calloc(ciphertext_buf_len, sizeof(uint8_t));
    if (plaintext_buf == NULL) {
        err = ESP_ERR_NO_MEM;
        goto exit;
    }

    esp_tee_sec_storage_aead_ctx_t ctx = {
        .key_id = key_id,
        .input = (uint8_t *)ciphertext_buf,
        .input_len = ciphertext_buf_len
    };

    err = esp_tee_sec_storage_aead_decrypt(&ctx, iv, sizeof(iv), tag, sizeof(tag), plaintext_buf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to decrypt data: %s", esp_err_to_name(err));
        goto exit;
    }

    char *plaintext = calloc(ciphertext_len + 1, sizeof(char));
    if (plaintext == NULL) {
        err = ESP_ERR_NO_MEM;
        goto exit;
    }
    hexbuf_to_hexstr(plaintext_buf, ciphertext_buf_len, plaintext, ciphertext_len + 1);

    ESP_LOGI(TAG, "Decrypted plaintext -\n%s", plaintext);

    free(ciphertext_buf);
    free(plaintext_buf);
    free(plaintext);

exit:
    return err;
}

void register_srv_sec_stg_decrypt(void)
{
    tee_sec_stg_decrypt_args.key_str_id = arg_str1(NULL, NULL, "<key_id>", "TEE Secure storage key ID");
    tee_sec_stg_decrypt_args.ciphertext = arg_str1(NULL, NULL, "<ciphertext>", "Ciphertext to be decrypted");
    tee_sec_stg_decrypt_args.iv = arg_str1(NULL, NULL, "<iv>", "AES-GCM initialization vector");
    tee_sec_stg_decrypt_args.tag = arg_str1(NULL, NULL, "<tag>", "AES-GCM authentication tag");
    tee_sec_stg_decrypt_args.end = arg_end(4);

    const esp_console_cmd_t tee_sec_stg = {
        .command = "tee_sec_stg_decrypt",
        .help = "Decrypt data using AES-GCM key with the given ID from secure storage",
        .hint = NULL,
        .func = &tee_sec_stg_decrypt,
        .argtable = &tee_sec_stg_decrypt_args,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&tee_sec_stg));
}
