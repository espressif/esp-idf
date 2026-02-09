/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>

#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_partition.h"

#include "esp_tee.h"
#include "esp_tee_sec_storage.h"
#include "secure_service_num.h"
#include "psa/initial_attestation.h"
#include "psa/crypto.h"
#include "psa_crypto_driver_esp_ecdsa.h"

#include "esp_random.h"
#include "nvs.h"
#include "unity.h"
#include "sdkconfig.h"

/* Note: negative value here so that assert message prints a grep-able
   error hex value (mbedTLS uses -N for error codes) */
#define TEST_ASSERT_MBEDTLS_OK(X)  TEST_ASSERT_EQUAL_HEX32(0, -(X))

#define SHA384_DIGEST_SZ         (48)
#define SHA256_DIGEST_SZ         (32)
#define ECDSA_SECP384R1_KEY_LEN  (48)
#define ECDSA_SECP256R1_KEY_LEN  (32)

#define MAX_SEC_STG_ITER         (16)

static const char *TAG = "test_esp_tee_sec_storage";

int verify_ecdsa_sign(const esp_tee_sec_storage_type_t key_type, const uint8_t *digest, size_t len, const esp_tee_sec_storage_ecdsa_pubkey_t *pubkey, const esp_tee_sec_storage_ecdsa_sign_t *sign)
{
#if SOC_ECDSA_SUPPORT_CURVE_P384 && !CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP384R1_SIGN
    TEST_ASSERT_FALSE(key_type == ESP_SEC_STG_KEY_ECDSA_SECP384R1);
#endif

    TEST_ASSERT_NOT_NULL(pubkey);
    TEST_ASSERT_NOT_NULL(digest);
    TEST_ASSERT_NOT_NULL(sign);
    TEST_ASSERT_NOT_EQUAL(0, len);

    int err = ESP_FAIL;

    psa_key_id_t key_id = 0;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));

    size_t pub_key_len;
    size_t signature_size;

    /* Use fixed-size array large enough for all supported curves (max: SECP384R1 = 2*48+1 = 97) */
    uint8_t pub_key[2 * ECDSA_SECP384R1_KEY_LEN + 1];

    switch (key_type) {
    case ESP_SEC_STG_KEY_ECDSA_SECP256R1:
        psa_set_key_bits(&key_attributes, ECDSA_SECP256R1_KEY_LEN * 8);
        pub_key_len = ECDSA_SECP256R1_KEY_LEN;
        signature_size = ECDSA_SECP256R1_KEY_LEN * 2;
        break;
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP384R1_SIGN
    case ESP_SEC_STG_KEY_ECDSA_SECP384R1:
        psa_set_key_bits(&key_attributes, ECDSA_SECP384R1_KEY_LEN * 8);
        pub_key_len = ECDSA_SECP384R1_KEY_LEN;
        signature_size = ECDSA_SECP384R1_KEY_LEN * 2;
        break;
#endif // CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP384R1_SIGN
    default:
        ESP_LOGE(TAG, "Unsupported ECDSA curve type");
        err = ESP_ERR_INVALID_ARG;
        goto exit;
    }
    pub_key[0] = 0x04;
    memcpy(pub_key + 1, pubkey->pub_x, pub_key_len);
    memcpy(pub_key + 1 + pub_key_len, pubkey->pub_y, pub_key_len);

    psa_status_t status = psa_import_key(&key_attributes, pub_key, 2 * pub_key_len + 1, &key_id);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to import ECDSA public key: %ld", status);
        err = ESP_ERR_INVALID_ARG;
        goto exit;
    }

    status = psa_verify_hash(key_id, PSA_ALG_ECDSA(PSA_ALG_SHA_256), digest, len, sign->signature, signature_size);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to verify ECDSA signature: %ld", status);
        err = ESP_ERR_INVALID_ARG;
        goto exit;
    }

    psa_destroy_key(key_id);
    psa_reset_key_attributes(&key_attributes);

    err = ESP_OK;

exit:
    return err;
}

TEST_CASE("Test TEE Secure Storage - Sign-verify (ecdsa_secp256r1)", "[sec_storage]")
{
    const size_t buf_sz = 16 * 1024 + 6;  // NOTE: Not an exact multiple of SHA block size
    unsigned char *message = heap_caps_malloc(buf_sz, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(message);

    esp_fill_random(message, buf_sz);

    uint8_t msg_digest[SHA256_DIGEST_SZ];
    size_t msg_digest_len = 0;
    psa_status_t status = psa_hash_compute(PSA_ALG_SHA_256, message, buf_sz, msg_digest, sizeof(msg_digest), &msg_digest_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
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
        TEST_ESP_OK(esp_tee_sec_storage_ecdsa_sign(&key_cfg, msg_digest, msg_digest_len, &sign));

        esp_tee_sec_storage_ecdsa_pubkey_t pubkey = {};
        TEST_ESP_OK(esp_tee_sec_storage_ecdsa_get_pubkey(&key_cfg, &pubkey));

        TEST_ESP_OK(verify_ecdsa_sign(key_cfg.type, msg_digest, msg_digest_len, &pubkey, &sign));

        TEST_ESP_OK(esp_tee_sec_storage_clear_key(key_cfg.id));
    }
}

#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP384R1_SIGN
TEST_CASE("Test TEE Secure Storage - Sign-verify (ecdsa_secp384r1)", "[sec_storage]")
{
    const size_t buf_sz = 16 * 1024 + 6;  // NOTE: Not an exact multiple of SHA block size
    unsigned char *message = heap_caps_malloc(buf_sz, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(message);

    esp_fill_random(message, buf_sz);

    uint8_t msg_digest[SHA384_DIGEST_SZ];
    // TEST_ASSERT_MBEDTLS_OK(mbedtls_sha512(message, buf_sz, msg_digest, false));
    size_t msg_digest_len = 0;
    psa_status_t status = psa_hash_compute(PSA_ALG_SHA_384, message, buf_sz, msg_digest, sizeof(msg_digest), &msg_digest_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    (void)msg_digest_len;
    free(message);

    esp_tee_sec_storage_key_cfg_t key_cfg = {
        .type = ESP_SEC_STG_KEY_ECDSA_SECP384R1
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

        TEST_ESP_OK(verify_ecdsa_sign(key_cfg.type, msg_digest, sizeof(msg_digest), &pubkey, &sign));

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
    size_t msg_digest_len = 0;
    psa_status_t status = psa_hash_compute(PSA_ALG_SHA_256, message, SZ, msg_digest, sizeof(msg_digest), &msg_digest_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
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
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_tee_sec_storage_ecdsa_sign(&key_cfg, msg_digest, sizeof(msg_digest), &sign));
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

static void do_ecdsa_sign_and_verify(const esp_tee_sec_storage_key_cfg_t *cfg, const uint8_t *digest, size_t digest_len)
{
    esp_tee_sec_storage_ecdsa_sign_t sign = {0};
    esp_tee_sec_storage_ecdsa_pubkey_t pubkey = {0};

    TEST_ESP_OK(esp_tee_sec_storage_ecdsa_sign(cfg, digest, digest_len, &sign));
    TEST_ESP_OK(esp_tee_sec_storage_ecdsa_get_pubkey(cfg, &pubkey));
    TEST_ESP_OK(verify_ecdsa_sign(cfg->type, digest, digest_len, &pubkey, &sign));
}

TEST_CASE("Test TEE Secure Storage - Host-generated keys", "[sec_storage_host_keygen]")
{
    const char *aes_key_ids[] = { "aes256_key0", "aes256_key1" };
    const char *ecdsa_key_id0 = "p256_key0";

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_tee_sec_storage_clear_key(aes_key_ids[0]));

    const size_t SZ = 100;
    uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(plaintext);
    esp_fill_random(plaintext, SZ);

    for (size_t i = 0; i < sizeof(aes_key_ids) / sizeof(aes_key_ids[0]); ++i) {
        test_aead_encrypt_decrypt(aes_key_ids[i], plaintext, SZ);
    }

    heap_caps_free(plaintext);

    TEST_ESP_OK(esp_tee_sec_storage_clear_key(aes_key_ids[1]));

    uint8_t digest_buf[SHA384_DIGEST_SZ];
    esp_fill_random(digest_buf, sizeof(digest_buf));

    esp_tee_sec_storage_key_cfg_t key_cfg = {
        .id   = ecdsa_key_id0,
        .type = ESP_SEC_STG_KEY_ECDSA_SECP256R1,
    };

    do_ecdsa_sign_and_verify(&key_cfg, digest_buf, SHA256_DIGEST_SZ);
    TEST_ESP_OK(esp_tee_sec_storage_clear_key(ecdsa_key_id0));

#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP384R1_SIGN
    const char *ecdsa_key_id2 = "p384_key0";
    key_cfg.id = ecdsa_key_id2;
    key_cfg.type = ESP_SEC_STG_KEY_ECDSA_SECP384R1;

    do_ecdsa_sign_and_verify(&key_cfg, digest_buf, SHA384_DIGEST_SZ);
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_tee_sec_storage_clear_key(ecdsa_key_id2));
#endif /* CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP384R1_SIGN */

#if CONFIG_SECURE_TEE_ATTESTATION
    // Prepare authentication challenge (just the nonce/challenge data)
    uint8_t auth_challenge[PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32];
    size_t challenge_size = sizeof(auth_challenge);
    esp_fill_random(auth_challenge, challenge_size);

    size_t token_buf_size = 0;
    TEST_ESP_OK(psa_initial_attest_get_token_size(challenge_size, &token_buf_size));

    uint8_t *token_buf = heap_caps_calloc(token_buf_size, sizeof(uint8_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(token_buf);

    size_t token_len = 0;
    TEST_ESP_OK(psa_initial_attest_get_token(auth_challenge, challenge_size, token_buf, token_buf_size, &token_len));
    free(token_buf);

    const char *attest_key_id  = "attest_key";
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_tee_sec_storage_clear_key(attest_key_id));
#endif /* CONFIG_SECURE_TEE_ATTESTATION */
}

#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
static const uint8_t sha[] = {
    0xda, 0x99, 0x5d, 0x66, 0xcb, 0xa5, 0xcb, 0xc7,
    0xc5, 0x3d, 0x8d, 0x08, 0x4d, 0x6a, 0x5e, 0x8e,
    0xf0, 0x84, 0x4b, 0x7b, 0xdd, 0xd0, 0x20, 0x47,
    0xa3, 0x6c, 0x61, 0xf3, 0xbc, 0xcf, 0xf4, 0xe2,
    0xf7, 0x70, 0xcd, 0x19, 0xf7, 0x31, 0x52, 0x48,
    0x1f, 0x29, 0x0e, 0x13, 0x24, 0x55, 0x9d, 0xb9,
    0x1a, 0x48, 0xa4, 0x0e, 0x75, 0xc2, 0xa8, 0x35,
    0xa9, 0x53, 0x97, 0xec, 0xcc, 0x66, 0x5b, 0xa6
};

static void test_ecdsa_sign(esp_ecdsa_curve_t curve)
{
    esp_tee_sec_storage_type_t key_type = ESP_SEC_STG_TYPE_MAX;
    size_t key_len = 0;
    size_t sha_len = SHA256_DIGEST_SZ;

    psa_algorithm_t sha_alg = 0;
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;

    uint8_t export_pub_key[1 + 2 * ECDSA_SECP384R1_KEY_LEN] = {0};
    uint8_t signature[2 * ECDSA_SECP384R1_KEY_LEN];
    size_t signature_len = 0;

    switch (curve) {
    case ESP_ECDSA_CURVE_SECP256R1:
        key_type = ESP_SEC_STG_KEY_ECDSA_SECP256R1;
        key_len  = ECDSA_SECP256R1_KEY_LEN;
        sha_alg = PSA_ALG_SHA_256;
        break;
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP384R1_SIGN
    case ESP_ECDSA_CURVE_SECP384R1:
        key_type = ESP_SEC_STG_KEY_ECDSA_SECP384R1;
        key_len  = ECDSA_SECP384R1_KEY_LEN;
        sha_len  = SHA384_DIGEST_SZ;
        sha_alg = PSA_ALG_SHA_384;
        break;
#endif
    default:
        ESP_LOGE(TAG, "Unsupported curve ID: %d", curve);
        return;
    }
    const char *key_id = "ecdsa_k0";

    esp_tee_sec_storage_key_cfg_t key_cfg = {
        .id = key_id,
        .type = key_type
    };

    esp_err_t err = esp_tee_sec_storage_clear_key(key_cfg.id);
    TEST_ASSERT_TRUE(err == ESP_OK || err == ESP_ERR_NOT_FOUND);

    // TODO: Add wrapper for psa_generate_key()
    TEST_ESP_OK(esp_tee_sec_storage_gen_key(&key_cfg));

    psa_key_id_t priv_key_id = 0;
    psa_key_attributes_t priv_key_attr = PSA_KEY_ATTRIBUTES_INIT;

    psa_key_id_t pub_key_id = 0;
    psa_key_attributes_t pub_key_attr = PSA_KEY_ATTRIBUTES_INIT;

    esp_ecdsa_opaque_key_t opaque_key = {
        .curve = curve,
        .tee_key_id = key_id,
    };

    psa_algorithm_t alg = PSA_ALG_ECDSA(sha_alg);
#if CONFIG_MBEDTLS_ECDSA_DETERMINISTIC
    alg = PSA_ALG_DETERMINISTIC_ECDSA(sha_alg);
#endif /* CONFIG_MBEDTLS_ECDSA_DETERMINISTIC */

    // Set attributes for opaque private key
    psa_set_key_type(&priv_key_attr, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&priv_key_attr, key_len * 8);
    psa_set_key_usage_flags(&priv_key_attr, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&priv_key_attr, alg);
    psa_set_key_lifetime(&priv_key_attr, PSA_KEY_LIFETIME_ESP_ECDSA_VOLATILE);  // Opaque key

    status = psa_import_key(&priv_key_attr, (uint8_t*) &opaque_key, sizeof(opaque_key), &priv_key_id);

    TEST_ASSERT_EQUAL_HEX32(PSA_SUCCESS, status);
    TEST_ASSERT_NOT_EQUAL(0, priv_key_id);

    size_t pub_key_len = 0;
    status = psa_export_public_key(priv_key_id, export_pub_key, sizeof(export_pub_key), &pub_key_len);

    TEST_ASSERT_EQUAL_HEX32(PSA_SUCCESS, status);
    TEST_ASSERT_EQUAL(pub_key_len, 1 + 2 * key_len);

    status = psa_sign_hash(priv_key_id,
                           alg,
                           sha, sha_len,
                           signature, 2 * key_len,
                           &signature_len);

    TEST_ASSERT_EQUAL_HEX32(PSA_SUCCESS, status);
    TEST_ASSERT_EQUAL(signature_len, 2 * key_len);

#if CONFIG_MBEDTLS_ECDSA_DETERMINISTIC
    uint8_t signature_det_verify[2 * ECDSA_SECP384R1_KEY_LEN];
    size_t signature_det_verify_len = 0;

    status = psa_sign_hash(priv_key_id,
                           alg,
                           sha, sha_len,
                           signature_det_verify, 2 * key_len,
                           &signature_det_verify_len);

    TEST_ASSERT_EQUAL_HEX32(PSA_SUCCESS, status);
    TEST_ASSERT_EQUAL(signature_det_verify_len, signature_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(signature, signature_det_verify, signature_len);
#endif

    psa_set_key_type(&pub_key_attr, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&pub_key_attr, key_len * 8);
    psa_set_key_usage_flags(&pub_key_attr, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&pub_key_attr, alg);
    psa_set_key_lifetime(&pub_key_attr, PSA_KEY_PERSISTENCE_VOLATILE);

    status = psa_import_key(&pub_key_attr, export_pub_key, pub_key_len, &pub_key_id);
    TEST_ASSERT_EQUAL_HEX32(PSA_SUCCESS, status);
    TEST_ASSERT_NOT_EQUAL(0, pub_key_id);

    status = psa_verify_hash(pub_key_id, alg, sha, sha_len, signature, 2 * key_len);
    TEST_ASSERT_EQUAL_HEX32(PSA_SUCCESS, status);

    TEST_ESP_OK(esp_tee_sec_storage_clear_key(key_cfg.id));

    psa_destroy_key(priv_key_id);
    psa_destroy_key(pub_key_id);
    psa_reset_key_attributes(&priv_key_attr);
    psa_reset_key_attributes(&pub_key_attr);
}

TEST_CASE("Test TEE Secure Storage - mbedtls ECDSA signing", "[mbedtls]")
{
    test_ecdsa_sign(MBEDTLS_ECP_DP_SECP256R1);
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP384R1_SIGN
    test_ecdsa_sign(ESP_ECDSA_CURVE_SECP384R1);
#endif
}
#endif
