/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>

#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_partition.h"

#include "mbedtls/ecp.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/sha256.h"
#include "ecdsa/ecdsa_alt.h"

#include "esp_tee.h"
#include "esp_tee_sec_storage.h"
#include "secure_service_num.h"
#if CONFIG_SECURE_TEE_ATTESTATION
#include "esp_tee_attestation.h"
#endif

#include "esp_random.h"
#include "nvs.h"
#include "unity.h"
#include "sdkconfig.h"

/* Note: negative value here so that assert message prints a grep-able
   error hex value (mbedTLS uses -N for error codes) */
#define TEST_ASSERT_MBEDTLS_OK(X)  TEST_ASSERT_EQUAL_HEX32(0, -(X))

#define SHA256_DIGEST_SZ         (32)
#define ECDSA_SECP256R1_KEY_LEN  (32)
#define ECDSA_SECP192R1_KEY_LEN  (24)

#define ESP_ATT_TK_BUF_SIZE      (1792)
#define ESP_ATT_TK_PSA_CERT_REF  ("0632793520245-10010")

#define MAX_SEC_STG_ITER         (16)

static const char *TAG = "test_esp_tee_sec_storage";

int verify_ecdsa_sign(const uint8_t *digest, size_t len, const esp_tee_sec_storage_ecdsa_pubkey_t *pubkey, const esp_tee_sec_storage_ecdsa_sign_t *sign, bool is_crv_p192)
{
#if !CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP192R1_SIGN
    TEST_ASSERT_FALSE(is_crv_p192);
#endif

    TEST_ASSERT_NOT_NULL(pubkey);
    TEST_ASSERT_NOT_NULL(digest);
    TEST_ASSERT_NOT_NULL(sign);
    TEST_ASSERT_NOT_EQUAL(0, len);

    mbedtls_mpi r, s;
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    mbedtls_ecdsa_context ecdsa_context;
    mbedtls_ecdsa_init(&ecdsa_context);

    mbedtls_ecp_group_id curve_id = MBEDTLS_ECP_DP_SECP256R1;
    if (is_crv_p192) {
        curve_id = MBEDTLS_ECP_DP_SECP192R1;
    }

    TEST_ASSERT_MBEDTLS_OK(mbedtls_ecp_group_load(&ecdsa_context.MBEDTLS_PRIVATE(grp), curve_id));
    size_t plen = mbedtls_mpi_size(&ecdsa_context.MBEDTLS_PRIVATE(grp).P);

    TEST_ASSERT_MBEDTLS_OK(mbedtls_mpi_read_binary(&r, sign->sign_r, plen));
    TEST_ASSERT_MBEDTLS_OK(mbedtls_mpi_read_binary(&s, sign->sign_s, plen));
    TEST_ASSERT_MBEDTLS_OK(mbedtls_mpi_read_binary(&ecdsa_context.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(X), pubkey->pub_x, plen));
    TEST_ASSERT_MBEDTLS_OK(mbedtls_mpi_read_binary(&ecdsa_context.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Y), pubkey->pub_y, plen));
    TEST_ASSERT_MBEDTLS_OK(mbedtls_mpi_lset(&ecdsa_context.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Z), 1));
    TEST_ASSERT_MBEDTLS_OK(mbedtls_ecdsa_verify(&ecdsa_context.MBEDTLS_PRIVATE(grp), digest, len, &ecdsa_context.MBEDTLS_PRIVATE(Q), &r, &s));

    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    mbedtls_ecdsa_free(&ecdsa_context);

    return 0;
}

TEST_CASE("Test TEE Secure Storage - Sign-verify (ecdsa_secp256r1)", "[sec_storage]")
{
    const size_t buf_sz = 16 * 1024 + 6;  // NOTE: Not an exact multiple of SHA block size
    unsigned char *message = heap_caps_malloc(buf_sz, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(message);

    esp_fill_random(message, buf_sz);

    uint8_t msg_digest[SHA256_DIGEST_SZ];
    TEST_ASSERT_MBEDTLS_OK(mbedtls_sha256(message, buf_sz, msg_digest, false));
    free(message);

    esp_tee_sec_storage_key_cfg_t key_cfg = {
        .type = ESP_SEC_STG_KEY_ECDSA_SECP256R1
    };

    for (unsigned int i = 0; i < MAX_SEC_STG_ITER; i++) {
        char key_id[32];
        int ret = snprintf(key_id, sizeof(key_id), "ecdsa_key_%u", i);
        TEST_ASSERT_TRUE(ret > 0 && ret < sizeof(key_id));

        key_cfg.id = key_id;
        esp_err_t err = esp_tee_sec_storage_clear_key(key_cfg.id);
        TEST_ASSERT_TRUE(err == ESP_OK || err == ESP_ERR_NOT_FOUND);
        TEST_ESP_OK(esp_tee_sec_storage_gen_key(&key_cfg));

        esp_tee_sec_storage_ecdsa_sign_t sign = {};
        TEST_ESP_OK(esp_tee_sec_storage_ecdsa_sign(&key_cfg, msg_digest, sizeof(msg_digest), &sign));

        esp_tee_sec_storage_ecdsa_pubkey_t pubkey = {};
        TEST_ESP_OK(esp_tee_sec_storage_ecdsa_get_pubkey(&key_cfg, &pubkey));

        TEST_ESP_OK(verify_ecdsa_sign(msg_digest, sizeof(msg_digest), &pubkey, &sign, false));

        TEST_ESP_OK(esp_tee_sec_storage_clear_key(key_cfg.id));
    }
}

#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP192R1_SIGN && !TEMPORARY_DISABLED_FOR_TARGETS(ESP32H2)
TEST_CASE("Test TEE Secure Storage - Sign-verify (ecdsa_secp192r1)", "[sec_storage]")
{
    const size_t buf_sz = 16 * 1024 + 6;  // NOTE: Not an exact multiple of SHA block size
    unsigned char *message = heap_caps_malloc(buf_sz, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(message);

    esp_fill_random(message, buf_sz);

    uint8_t msg_digest[SHA256_DIGEST_SZ];
    TEST_ASSERT_MBEDTLS_OK(mbedtls_sha256(message, buf_sz, msg_digest, false));
    free(message);

    esp_tee_sec_storage_key_cfg_t key_cfg = {
        .type = ESP_SEC_STG_KEY_ECDSA_SECP192R1
    };

    for (unsigned int i = 0; i < MAX_SEC_STG_ITER; i++) {
        char key_id[32];
        int ret = snprintf(key_id, sizeof(key_id), "ecdsa_key_%u", i);
        TEST_ASSERT_TRUE(ret > 0 && ret < sizeof(key_id));

        key_cfg.id = key_id;
        esp_err_t err = esp_tee_sec_storage_clear_key(key_cfg.id);
        TEST_ASSERT_TRUE(err == ESP_OK || err == ESP_ERR_NOT_FOUND);
        TEST_ESP_OK(esp_tee_sec_storage_gen_key(&key_cfg));

        esp_tee_sec_storage_ecdsa_sign_t sign = {};
        TEST_ESP_OK(esp_tee_sec_storage_ecdsa_sign(&key_cfg, msg_digest, sizeof(msg_digest), &sign));

        esp_tee_sec_storage_ecdsa_pubkey_t pubkey = {};
        TEST_ESP_OK(esp_tee_sec_storage_ecdsa_get_pubkey(&key_cfg, &pubkey));

        TEST_ESP_OK(verify_ecdsa_sign(msg_digest, sizeof(msg_digest), &pubkey, &sign, true));

        TEST_ESP_OK(esp_tee_sec_storage_clear_key(key_cfg.id));
    }
}
#endif

TEST_CASE("Test TEE Secure Storage - Encrypt-decrypt (aes256_gcm)", "[sec_storage]")
{
    const uint8_t SZ = 100;
    uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(plaintext);
    esp_fill_random(plaintext, SZ);

    uint8_t *ciphertext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(ciphertext);

    uint8_t *decryptedtext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    uint8_t tag[12];
    uint8_t aad[16];
    memset(aad, 0xA5, sizeof(aad));

    esp_tee_sec_storage_key_cfg_t key_cfg = {
        .type = ESP_SEC_STG_KEY_AES256
    };

    for (unsigned int i = 0; i < MAX_SEC_STG_ITER; i++) {
        char test_key_id[32];
        int ret = snprintf(test_key_id, sizeof(test_key_id), "aes_key_%u", i);
        TEST_ASSERT_TRUE(ret > 0 && ret < sizeof(test_key_id));

        key_cfg.id = test_key_id;
        esp_err_t err = esp_tee_sec_storage_clear_key(key_cfg.id);
        TEST_ASSERT_TRUE(err == ESP_OK || err == ESP_ERR_NOT_FOUND);
        TEST_ESP_OK(esp_tee_sec_storage_gen_key(&key_cfg));

        esp_tee_sec_storage_aead_ctx_t aead_ctx = {
            .key_id = test_key_id,
            .aad = aad,
            .aad_len = sizeof(aad),
        };

        aead_ctx.input = plaintext;
        aead_ctx.input_len = SZ;
        TEST_ESP_OK(esp_tee_sec_storage_aead_encrypt(&aead_ctx, tag, sizeof(tag), ciphertext));

        aead_ctx.input = ciphertext;
        aead_ctx.input_len = SZ;
        TEST_ESP_OK(esp_tee_sec_storage_aead_decrypt(&aead_ctx, tag, sizeof(tag), decryptedtext));

        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

        TEST_ESP_OK(esp_tee_sec_storage_clear_key(key_cfg.id));
    }

    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("Test TEE Secure Storage - Operations with invalid/non-existent keys", "[sec_storage]")
{
    // Setup for ECDSA
    const uint8_t SZ = 100;
    unsigned char *message = heap_caps_malloc(SZ, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(message);
    esp_fill_random(message, SZ);
    uint8_t msg_digest[SHA256_DIGEST_SZ];
    TEST_ASSERT_MBEDTLS_OK(mbedtls_sha256(message, SZ, msg_digest, false));
    free(message);

    const char *key_id = "key_id_misc";
    esp_tee_sec_storage_key_cfg_t key_cfg = {
        .id = key_id,
        .type = ESP_SEC_STG_KEY_ECDSA_SECP256R1
    };

    // Setup for AES
    uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(plaintext);
    esp_fill_random(plaintext, SZ);
    uint8_t *ciphertext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(ciphertext);
    uint8_t tag[12];
    uint8_t aad[16];
    memset(aad, 0xA5, sizeof(aad));

    esp_tee_sec_storage_aead_ctx_t aead_ctx = {
        .key_id = key_id,
        .aad = aad,
        .aad_len = sizeof(aad),
        .input = plaintext,
        .input_len = SZ
    };

    // Test ECDSA key with AES operation
    ESP_LOGI(TAG, "Key ID: %s - Trying AES operation with ECDSA key...", key_cfg.id);
    esp_err_t err = esp_tee_sec_storage_clear_key(key_cfg.id);
    ESP_LOGW(TAG, "err: %x", err);
    TEST_ASSERT_TRUE(err == ESP_OK || err == ESP_ERR_NOT_FOUND);
    TEST_ESP_OK(esp_tee_sec_storage_gen_key(&key_cfg));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_tee_sec_storage_aead_encrypt(&aead_ctx, tag, sizeof(tag), ciphertext));

    TEST_ESP_OK(esp_tee_sec_storage_clear_key(key_cfg.id));

    // Test AES key with ECDSA operation
    ESP_LOGI(TAG, "Key ID: %s - Trying ECDSA operation with AES key...", key_cfg.id);
    TEST_ASSERT_TRUE(err == ESP_OK || err == ESP_ERR_NOT_FOUND);
    key_cfg.type = ESP_SEC_STG_KEY_AES256;
    TEST_ESP_OK(esp_tee_sec_storage_gen_key(&key_cfg));

    esp_tee_sec_storage_ecdsa_sign_t sign = {};
    TEST_ESP_ERR(ESP_FAIL, esp_tee_sec_storage_ecdsa_sign(&key_cfg, msg_digest, sizeof(msg_digest), &sign));
    TEST_ESP_OK(esp_tee_sec_storage_clear_key(key_cfg.id));

    // Test with non-existent data
    TEST_ESP_ERR(ESP_ERR_NVS_NOT_FOUND, esp_tee_sec_storage_aead_encrypt(&aead_ctx, tag, sizeof(tag), ciphertext));

    free(plaintext);
    free(ciphertext);
}

TEST_CASE("Test TEE Secure Storage - Null Pointer and Zero Length", "[sec_storage]")
{
    const char *key_id = "key_id_misc";
    uint8_t data[31], tag[12];

    esp_tee_sec_storage_key_cfg_t key_cfg = {
        .id = key_id,
        .type = ESP_SEC_STG_KEY_AES256
    };

    esp_err_t err = esp_tee_sec_storage_clear_key(key_cfg.id);
    TEST_ASSERT_TRUE(err == ESP_OK || err == ESP_ERR_NOT_FOUND);
    TEST_ESP_OK(esp_tee_sec_storage_gen_key(&key_cfg));

    esp_tee_sec_storage_aead_ctx_t aead_ctx = {
        .key_id = key_cfg.id,
        .input = data,
        .input_len = sizeof(data),
    };

    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_tee_sec_storage_aead_encrypt(&aead_ctx, NULL, sizeof(tag), data));
    TEST_ESP_ERR(ESP_ERR_INVALID_SIZE, esp_tee_sec_storage_aead_encrypt(&aead_ctx, tag, 0, data));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_tee_sec_storage_aead_decrypt(&aead_ctx, NULL, sizeof(tag), data));
    TEST_ESP_ERR(ESP_ERR_INVALID_SIZE, esp_tee_sec_storage_aead_decrypt(&aead_ctx, tag, 0, data));

    aead_ctx.input = NULL;
    aead_ctx.input_len = sizeof(data);
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_tee_sec_storage_aead_encrypt(&aead_ctx, tag, sizeof(tag), data));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_tee_sec_storage_aead_decrypt(&aead_ctx, tag, sizeof(tag), data));

    aead_ctx.input = data;
    aead_ctx.input_len = 0;
    TEST_ESP_ERR(ESP_ERR_INVALID_SIZE, esp_tee_sec_storage_aead_encrypt(&aead_ctx, tag, sizeof(tag), data));
    TEST_ESP_ERR(ESP_ERR_INVALID_SIZE, esp_tee_sec_storage_aead_decrypt(&aead_ctx, tag, sizeof(tag), data));

    TEST_ESP_OK(esp_tee_sec_storage_clear_key(key_id));

    key_cfg.type = ESP_SEC_STG_KEY_ECDSA_SECP256R1;
    err = esp_tee_sec_storage_clear_key(key_cfg.id);
    TEST_ASSERT_TRUE(err == ESP_OK || err == ESP_ERR_NOT_FOUND);
    TEST_ESP_OK(esp_tee_sec_storage_gen_key(&key_cfg));

    esp_tee_sec_storage_ecdsa_sign_t sign = {};
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_tee_sec_storage_ecdsa_sign(&key_cfg, NULL, sizeof(data), &sign));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_tee_sec_storage_ecdsa_sign(&key_cfg, data, 0, &sign));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_tee_sec_storage_ecdsa_sign(&key_cfg, data, sizeof(data), NULL));

    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_tee_sec_storage_ecdsa_get_pubkey(&key_cfg, NULL));

    TEST_ESP_OK(esp_tee_sec_storage_clear_key(key_cfg.id));
}

TEST_CASE("Test TEE Secure Storage - WRITE_ONCE keys", "[sec_storage]")
{
    const char *key_id = "key_id_test_wo";

    esp_tee_sec_storage_key_cfg_t key_cfg = {
        .id = key_id,
        .type = ESP_SEC_STG_KEY_AES256,
        .flags = SEC_STORAGE_FLAG_WRITE_ONCE,
    };

    esp_err_t err = esp_tee_sec_storage_clear_key(key_cfg.id);
    TEST_ASSERT_TRUE(err == ESP_OK || err == ESP_ERR_NOT_FOUND);

    TEST_ESP_OK(esp_tee_sec_storage_gen_key(&key_cfg));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_tee_sec_storage_gen_key(&key_cfg));

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_tee_sec_storage_clear_key(key_cfg.id));
}

static void test_aead_encrypt_decrypt(const char *key_id, const uint8_t *input, size_t len)
{
    uint8_t *ciphertext = heap_caps_malloc(len, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(ciphertext);
    uint8_t *decrypted = heap_caps_malloc(len, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(decrypted);

    uint8_t tag[12];
    uint8_t aad[16];
    esp_fill_random(aad, sizeof(aad));

    esp_tee_sec_storage_aead_ctx_t ctx = {
        .key_id = key_id,
        .aad = aad,
        .aad_len = sizeof(aad),
    };

    ctx.input = input;
    ctx.input_len = len;
    TEST_ESP_OK(esp_tee_sec_storage_aead_encrypt(&ctx, tag, sizeof(tag), ciphertext));

    ctx.input = ciphertext;
    ctx.input_len = len;
    TEST_ESP_OK(esp_tee_sec_storage_aead_decrypt(&ctx, tag, sizeof(tag), decrypted));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(input, decrypted, len);

    free(ciphertext);
    free(decrypted);
}

TEST_CASE("Test TEE Secure Storage - Host-generated keys", "[sec_storage_host_keygen]")
{
    const char *aes_key_id0 = "aes256_key0";
    const char *aes_key_id1 = "aes256_key1";
    const char *ecdsa_key_id0 = "p256_key0";
    const char *attest_key_id = "attest_key";

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_tee_sec_storage_clear_key(aes_key_id0));

    const size_t SZ = 100;
    uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(plaintext);
    esp_fill_random(plaintext, SZ);

    test_aead_encrypt_decrypt(aes_key_id0, plaintext, SZ);
    test_aead_encrypt_decrypt(aes_key_id1, plaintext, SZ);

    free(plaintext);

    TEST_ESP_OK(esp_tee_sec_storage_clear_key(aes_key_id1));

    uint8_t msg_digest[SHA256_DIGEST_SZ];
    esp_fill_random(msg_digest, sizeof(msg_digest));

    esp_tee_sec_storage_key_cfg_t key_cfg = {
        .id = ecdsa_key_id0,
        .type = ESP_SEC_STG_KEY_ECDSA_SECP256R1,
    };

    esp_tee_sec_storage_ecdsa_sign_t sign = {};
    TEST_ESP_OK(esp_tee_sec_storage_ecdsa_sign(&key_cfg, msg_digest, sizeof(msg_digest), &sign));

    esp_tee_sec_storage_ecdsa_pubkey_t pubkey = {};
    TEST_ESP_OK(esp_tee_sec_storage_ecdsa_get_pubkey(&key_cfg, &pubkey));

    TEST_ESP_OK(verify_ecdsa_sign(msg_digest, sizeof(msg_digest), &pubkey, &sign, false));

    TEST_ESP_OK(esp_tee_sec_storage_clear_key(ecdsa_key_id0));

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_tee_sec_storage_clear_key(attest_key_id));

#if CONFIG_SECURE_TEE_ATTESTATION
    uint8_t *token_buf = heap_caps_calloc(ESP_ATT_TK_BUF_SIZE, sizeof(uint8_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(token_buf);

    uint32_t token_len = 0;
    TEST_ESP_OK(esp_tee_att_generate_token(0xA1B2C3D4, 0x0FACADE0, (const char *)ESP_ATT_TK_PSA_CERT_REF,
                                           token_buf, ESP_ATT_TK_BUF_SIZE, &token_len));

    free(token_buf);
#endif
}

#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
static const uint8_t sha[] = {
    0x0c, 0xaa, 0x08, 0xb4, 0xf0, 0x89, 0xd3, 0x45,
    0xbb, 0x55, 0x98, 0xd9, 0xc2, 0xe9, 0x65, 0x5d,
    0x7e, 0xa3, 0xa9, 0xc3, 0xcd, 0x69, 0xb1, 0xcf,
    0x91, 0xbe, 0x58, 0x10, 0xfe, 0x80, 0x65, 0x6e
};

static void test_ecdsa_sign(mbedtls_ecp_group_id gid)
{
    bool is_crv_p192 = false;
    esp_tee_sec_storage_type_t key_type = ESP_SEC_STG_KEY_ECDSA_SECP256R1;
    size_t key_len = ECDSA_SECP256R1_KEY_LEN;

    if (gid == MBEDTLS_ECP_DP_SECP192R1) {
        is_crv_p192 = true;
        key_type = ESP_SEC_STG_KEY_ECDSA_SECP192R1;
        key_len = ECDSA_SECP192R1_KEY_LEN;
    }

    const char *key_id = "ecdsa_k0";

    esp_tee_sec_storage_key_cfg_t key_cfg = {
        .id = key_id,
        .type = key_type
    };

    esp_err_t err = esp_tee_sec_storage_clear_key(key_cfg.id);
    TEST_ASSERT_TRUE(err == ESP_OK || err == ESP_ERR_NOT_FOUND);

    TEST_ESP_OK(esp_tee_sec_storage_gen_key(&key_cfg));

    esp_tee_sec_storage_ecdsa_pubkey_t pubkey = {};
    TEST_ESP_OK(esp_tee_sec_storage_ecdsa_get_pubkey(&key_cfg, &pubkey));

    mbedtls_mpi r, s;
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    mbedtls_ecdsa_context ecdsa_context;
    mbedtls_ecdsa_init(&ecdsa_context);

    TEST_ASSERT_MBEDTLS_OK(mbedtls_ecp_group_load(&ecdsa_context.MBEDTLS_PRIVATE(grp), gid));

    mbedtls_pk_context key_ctx;

    esp_ecdsa_pk_conf_t conf = {
        .grp_id = gid,
        .tee_key_id = key_id,
        .load_pubkey = true,
        .use_tee_sec_stg_key = true,
    };
    TEST_ASSERT_EQUAL(0, esp_ecdsa_tee_set_pk_context(&key_ctx, &conf));

    mbedtls_ecp_keypair *keypair = mbedtls_pk_ec(key_ctx);
    mbedtls_mpi key_mpi = keypair->MBEDTLS_PRIVATE(d);

    TEST_ASSERT_MBEDTLS_OK(mbedtls_ecdsa_sign(&ecdsa_context.MBEDTLS_PRIVATE(grp), &r, &s, &key_mpi, sha, SHA256_DIGEST_SZ, NULL, NULL));

    esp_tee_sec_storage_ecdsa_sign_t sign = {};
    TEST_ASSERT_MBEDTLS_OK(mbedtls_mpi_write_binary(&r, sign.sign_r, key_len));
    TEST_ASSERT_MBEDTLS_OK(mbedtls_mpi_write_binary(&s, sign.sign_s, key_len));

    TEST_ESP_OK(verify_ecdsa_sign(sha, sizeof(sha), &pubkey, &sign, is_crv_p192));

    mbedtls_pk_free(&key_ctx);
    mbedtls_ecdsa_free(&ecdsa_context);
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);

    TEST_ESP_OK(esp_tee_sec_storage_clear_key(key_cfg.id));
}

TEST_CASE("Test TEE Secure Storage - mbedtls ECDSA signing", "[mbedtls]")
{
    test_ecdsa_sign(MBEDTLS_ECP_DP_SECP256R1);
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP192R1_SIGN && !TEMPORARY_DISABLED_FOR_TARGETS(ESP32H2)
    test_ecdsa_sign(MBEDTLS_ECP_DP_SECP192R1);
#endif
}
#endif
