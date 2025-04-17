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

#include "esp_random.h"

#include "unity.h"
#include "sdkconfig.h"

/* Note: negative value here so that assert message prints a grep-able
   error hex value (mbedTLS uses -N for error codes) */
#define TEST_ASSERT_MBEDTLS_OK(X)  TEST_ASSERT_EQUAL_HEX32(0, -(X))

#define SHA256_DIGEST_SZ         (32)
#define ECDSA_SECP256R1_KEY_LEN  (32)
#define ECDSA_SECP192R1_KEY_LEN  (24)

static const char *TAG = "test_esp_tee_sec_storage";

int verify_ecdsa_sign(const uint8_t *digest, size_t len, const esp_tee_sec_storage_pubkey_t *pubkey, const esp_tee_sec_storage_sign_t *sign, bool is_crv_p192)
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

TEST_CASE("Test TEE Secure Storage - Sign-verify (ecdsa_secp256r1) with all key-slot IDs", "[secure_storage]")
{
    const size_t buf_sz = 16 * 1024 + 6;  // NOTE: Not an exact multiple of SHA block size
    unsigned char *message = heap_caps_malloc(buf_sz, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(message);

    esp_fill_random(message, buf_sz);

    uint8_t msg_digest[SHA256_DIGEST_SZ];
    TEST_ASSERT_MBEDTLS_OK(mbedtls_sha256(message, buf_sz, msg_digest, false));
    free(message);

    TEST_ESP_OK(esp_tee_sec_storage_init());

    for (uint16_t slot_id = 0; slot_id <= MAX_SEC_STG_SLOT_ID; slot_id++) {
        TEST_ESP_OK(esp_tee_sec_storage_clear_slot(slot_id));
        TEST_ESP_OK(esp_tee_sec_storage_gen_key(slot_id, ESP_SEC_STG_KEY_ECDSA_SECP256R1));

        esp_tee_sec_storage_sign_t sign = {};
        TEST_ESP_OK(esp_tee_sec_storage_get_signature(slot_id, ESP_SEC_STG_KEY_ECDSA_SECP256R1, msg_digest, sizeof(msg_digest), &sign));

        esp_tee_sec_storage_pubkey_t pubkey = {};
        TEST_ESP_OK(esp_tee_sec_storage_get_pubkey(slot_id, ESP_SEC_STG_KEY_ECDSA_SECP256R1, &pubkey));

        TEST_ESP_OK(verify_ecdsa_sign(msg_digest, sizeof(msg_digest), &pubkey, &sign, false));

        TEST_ESP_OK(esp_tee_sec_storage_clear_slot(slot_id));
        TEST_ASSERT_TRUE(esp_tee_sec_storage_is_slot_empty(slot_id));
    }
}

#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP192R1_SIGN
TEST_CASE("Test TEE Secure Storage - Sign-verify (ecdsa_secp192r1) with all key-slot IDs", "[secure_storage]")
{
    const size_t buf_sz = 16 * 1024 + 6;  // NOTE: Not an exact multiple of SHA block size
    unsigned char *message = heap_caps_malloc(buf_sz, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(message);

    esp_fill_random(message, buf_sz);

    uint8_t msg_digest[SHA256_DIGEST_SZ];
    TEST_ASSERT_MBEDTLS_OK(mbedtls_sha256(message, buf_sz, msg_digest, false));
    free(message);

    TEST_ESP_OK(esp_tee_sec_storage_init());

    for (uint16_t slot_id = 0; slot_id <= MAX_SEC_STG_SLOT_ID; slot_id++) {
        TEST_ESP_OK(esp_tee_sec_storage_clear_slot(slot_id));
        TEST_ESP_OK(esp_tee_sec_storage_gen_key(slot_id, ESP_SEC_STG_KEY_ECDSA_SECP192R1));

        esp_tee_sec_storage_sign_t sign = {};
        TEST_ESP_OK(esp_tee_sec_storage_get_signature(slot_id, ESP_SEC_STG_KEY_ECDSA_SECP192R1, msg_digest, sizeof(msg_digest), &sign));

        esp_tee_sec_storage_pubkey_t pubkey = {};
        TEST_ESP_OK(esp_tee_sec_storage_get_pubkey(slot_id, ESP_SEC_STG_KEY_ECDSA_SECP192R1, &pubkey));

        TEST_ESP_OK(verify_ecdsa_sign(msg_digest, sizeof(msg_digest), &pubkey, &sign, true));

        TEST_ESP_OK(esp_tee_sec_storage_clear_slot(slot_id));
        TEST_ASSERT_TRUE(esp_tee_sec_storage_is_slot_empty(slot_id));
    }
}
#endif

TEST_CASE("Test TEE Secure Storage - Encrypt-decrypt (aes256_gcm) with all key-slot IDs", "[secure_storage]")
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

    TEST_ESP_OK(esp_tee_sec_storage_init());

    for (uint16_t slot_id = 0; slot_id <= MAX_SEC_STG_SLOT_ID; slot_id++) {
        TEST_ESP_OK(esp_tee_sec_storage_clear_slot(slot_id));
        TEST_ESP_OK(esp_tee_sec_storage_gen_key(slot_id, ESP_SEC_STG_KEY_AES256));

        TEST_ESP_OK(esp_tee_sec_storage_encrypt(slot_id, plaintext, SZ, aad, sizeof(aad),
                                                tag, sizeof(tag), ciphertext));
        TEST_ESP_OK(esp_tee_sec_storage_decrypt(slot_id, ciphertext, SZ, aad, sizeof(aad),
                                                tag, sizeof(tag), decryptedtext));

        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

        TEST_ESP_OK(esp_tee_sec_storage_clear_slot(slot_id));
        TEST_ASSERT_TRUE(esp_tee_sec_storage_is_slot_empty(slot_id));
    }

    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("Test TEE Secure Storage - Operations with invalid/non-existent keys", "[secure_storage]")
{
    // Setup for ECDSA
    const uint8_t SZ = 100;
    unsigned char *message = heap_caps_malloc(SZ, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(message);
    esp_fill_random(message, SZ);
    uint8_t msg_digest[SHA256_DIGEST_SZ];
    TEST_ASSERT_MBEDTLS_OK(mbedtls_sha256(message, SZ, msg_digest, false));
    free(message);

    // Setup for AES
    uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(plaintext);
    esp_fill_random(plaintext, SZ);
    uint8_t *ciphertext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(ciphertext);
    uint8_t tag[12];
    uint8_t aad[16];
    memset(aad, 0xA5, sizeof(aad));

    TEST_ESP_OK(esp_tee_sec_storage_init());

    // Test ECDSA key with AES operation
    const uint16_t slot_id = 7;
    ESP_LOGI(TAG, "Slot ID: %u - Trying AES operation with ECDSA key...", slot_id);
    TEST_ESP_OK(esp_tee_sec_storage_clear_slot(slot_id));

    esp_tee_sec_storage_type_t key_type = ESP_SEC_STG_KEY_ECDSA_SECP256R1;
    TEST_ESP_OK(esp_tee_sec_storage_gen_key(slot_id, key_type));

    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, esp_tee_sec_storage_encrypt(slot_id, plaintext, SZ, aad, sizeof(aad),
                                                                tag, sizeof(tag), ciphertext));

    // Test AES key with ECDSA operation
    ESP_LOGI(TAG, "Slot ID: %u - Trying ECDSA operation with AES key...", slot_id);
    TEST_ESP_OK(esp_tee_sec_storage_clear_slot(slot_id));
    TEST_ESP_OK(esp_tee_sec_storage_gen_key(slot_id, ESP_SEC_STG_KEY_AES256));

    esp_tee_sec_storage_sign_t sign = {};
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, esp_tee_sec_storage_get_signature(slot_id, key_type, msg_digest, sizeof(msg_digest), &sign));

    TEST_ESP_OK(esp_tee_sec_storage_clear_slot(slot_id));
    TEST_ASSERT_TRUE(esp_tee_sec_storage_is_slot_empty(slot_id));

    // Test with non-existent data
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, esp_tee_sec_storage_encrypt(slot_id, plaintext, SZ, aad, sizeof(aad),
                                                                tag, sizeof(tag), ciphertext));

    free(plaintext);
    free(ciphertext);
}

TEST_CASE("Test TEE Secure Storage - Invalid key-slot IDs", "[secure_storage]")
{
    TEST_ESP_OK(esp_tee_sec_storage_init());

    uint16_t slot_id = MAX_SEC_STG_SLOT_ID + 1;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_tee_sec_storage_gen_key(slot_id, ESP_SEC_STG_KEY_ECDSA_SECP256R1));

    slot_id = MIN_SEC_STG_SLOT_ID - 1;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_tee_sec_storage_gen_key(slot_id, ESP_SEC_STG_KEY_AES256));
}

TEST_CASE("Test TEE Secure Storage - Exhaust all key-slots", "[secure_storage]")
{
    TEST_ESP_OK(esp_tee_sec_storage_init());

    esp_err_t err = ESP_FAIL;
    uint16_t slot_id = MIN_SEC_STG_SLOT_ID;

    while (1) {
        esp_tee_sec_storage_clear_slot(slot_id);
        err = esp_tee_sec_storage_gen_key(slot_id, ESP_SEC_STG_KEY_AES256);
        if (err != ESP_OK) {
            break;
        }
        TEST_ASSERT_FALSE(esp_tee_sec_storage_is_slot_empty(slot_id));
        slot_id++;
    }
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_tee_sec_storage_gen_key(MAX_SEC_STG_SLOT_ID, ESP_SEC_STG_KEY_AES256));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_tee_sec_storage_gen_key(MIN_SEC_STG_SLOT_ID, ESP_SEC_STG_KEY_AES256));

    uint16_t mid_slot = (MIN_SEC_STG_SLOT_ID + MAX_SEC_STG_SLOT_ID) / 2;
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_tee_sec_storage_gen_key(mid_slot, ESP_SEC_STG_KEY_AES256));
}

TEST_CASE("Test TEE Secure Storage - Null Pointer and Zero Length", "[secure_storage]")
{
    TEST_ESP_OK(esp_tee_sec_storage_init());

    const uint16_t slot_id = 7;
    uint8_t data[31], tag[12];

    TEST_ESP_OK(esp_tee_sec_storage_clear_slot(slot_id));
    TEST_ESP_OK(esp_tee_sec_storage_gen_key(slot_id, ESP_SEC_STG_KEY_AES256));

    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_tee_sec_storage_encrypt(slot_id, NULL, sizeof(data), NULL, 0, tag, sizeof(tag), data));
    TEST_ESP_ERR(ESP_ERR_INVALID_SIZE, esp_tee_sec_storage_encrypt(slot_id, data, 0, NULL, 0, tag, sizeof(tag), data));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_tee_sec_storage_encrypt(slot_id, data, sizeof(data), NULL, 0, NULL, sizeof(tag), data));
    TEST_ESP_ERR(ESP_ERR_INVALID_SIZE, esp_tee_sec_storage_encrypt(slot_id, data, sizeof(data), NULL, 0, tag, 0, data));

    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_tee_sec_storage_decrypt(slot_id, NULL, sizeof(data), NULL, 0, tag, sizeof(tag), data));
    TEST_ESP_ERR(ESP_ERR_INVALID_SIZE, esp_tee_sec_storage_decrypt(slot_id, data, 0, NULL, 0, tag, sizeof(tag), data));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_tee_sec_storage_decrypt(slot_id, data, sizeof(data), NULL, 0, NULL, sizeof(tag), data));
    TEST_ESP_ERR(ESP_ERR_INVALID_SIZE, esp_tee_sec_storage_decrypt(slot_id, data, sizeof(data), NULL, 0, tag, 0, data));

    TEST_ESP_OK(esp_tee_sec_storage_clear_slot(slot_id));

    esp_tee_sec_storage_type_t key_type = ESP_SEC_STG_KEY_ECDSA_SECP256R1;
    TEST_ESP_OK(esp_tee_sec_storage_gen_key(slot_id, key_type));

    esp_tee_sec_storage_sign_t sign = {};
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_tee_sec_storage_get_signature(slot_id, key_type, NULL, sizeof(data), &sign));
    TEST_ESP_ERR(ESP_ERR_INVALID_SIZE, esp_tee_sec_storage_get_signature(slot_id, key_type, data, 0, &sign));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_tee_sec_storage_get_signature(slot_id, key_type, data, sizeof(data), NULL));

    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_tee_sec_storage_get_pubkey(slot_id, key_type, NULL));

    TEST_ESP_OK(esp_tee_sec_storage_clear_slot(slot_id));
}

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C6)
TEST_CASE("Test TEE Secure Storage - Corruption from non-secure world", "[secure_storage_neg]")
{
    const esp_partition_t *tee_sec_stg = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_TEE_SEC_STORAGE, NULL);
    TEST_ASSERT_NOT_NULL(tee_sec_stg);

    uint8_t buf_w[128];
    memset(buf_w, 0xA5, sizeof(buf_w));
    TEST_ESP_OK(esp_partition_write(tee_sec_stg, 0x00, buf_w, sizeof(buf_w)));

    uint8_t buf_r[128];
    memset(buf_r, 0x00, sizeof(buf_r));
    TEST_ESP_OK(esp_partition_read(tee_sec_stg, 0x00, buf_r, sizeof(buf_r)));
    ESP_LOG_BUFFER_HEXDUMP(TAG, buf_r, sizeof(buf_r), ESP_LOG_INFO);

    TEST_FAIL_MESSAGE("APM violation interrupt should have been generated");
}
#endif

#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
static const uint8_t sha[] = {
    0x0c, 0xaa, 0x08, 0xb4, 0xf0, 0x89, 0xd3, 0x45,
    0xbb, 0x55, 0x98, 0xd9, 0xc2, 0xe9, 0x65, 0x5d,
    0x7e, 0xa3, 0xa9, 0xc3, 0xcd, 0x69, 0xb1, 0xcf,
    0x91, 0xbe, 0x58, 0x10, 0xfe, 0x80, 0x65, 0x6e
};

static void test_ecdsa_sign(mbedtls_ecp_group_id gid, const uint8_t *hash, int slot_id)
{
    TEST_ESP_OK(esp_tee_sec_storage_init());
    TEST_ESP_OK(esp_tee_sec_storage_clear_slot(slot_id));

    bool is_crv_p192 = false;
    esp_tee_sec_storage_type_t key_type = ESP_SEC_STG_KEY_ECDSA_SECP256R1;
    size_t key_len = ECDSA_SECP256R1_KEY_LEN;

    if (gid == MBEDTLS_ECP_DP_SECP192R1) {
        is_crv_p192 = true;
        key_type = ESP_SEC_STG_KEY_ECDSA_SECP192R1;
        key_len = ECDSA_SECP192R1_KEY_LEN;
    }

    TEST_ESP_OK(esp_tee_sec_storage_gen_key(slot_id, key_type));

    esp_tee_sec_storage_pubkey_t pubkey = {};
    TEST_ESP_OK(esp_tee_sec_storage_get_pubkey(slot_id, key_type, &pubkey));

    mbedtls_mpi r, s;
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    mbedtls_ecdsa_context ecdsa_context;
    mbedtls_ecdsa_init(&ecdsa_context);

    TEST_ASSERT_MBEDTLS_OK(mbedtls_ecp_group_load(&ecdsa_context.MBEDTLS_PRIVATE(grp), gid));

    mbedtls_pk_context key_ctx;

    esp_ecdsa_pk_conf_t conf = {
        .grp_id = gid,
        .tee_slot_id = slot_id,
        .load_pubkey = true,
        .use_tee_sec_stg_key = true,
    };
    TEST_ASSERT_EQUAL(0, esp_ecdsa_tee_set_pk_context(&key_ctx, &conf));

    mbedtls_ecp_keypair *keypair = mbedtls_pk_ec(key_ctx);
    mbedtls_mpi key_mpi = keypair->MBEDTLS_PRIVATE(d);

    TEST_ASSERT_MBEDTLS_OK(mbedtls_ecdsa_sign(&ecdsa_context.MBEDTLS_PRIVATE(grp), &r, &s, &key_mpi, sha, SHA256_DIGEST_SZ, NULL, NULL));

    esp_tee_sec_storage_sign_t sign = {};
    TEST_ASSERT_MBEDTLS_OK(mbedtls_mpi_write_binary(&r, sign.sign_r, key_len));
    TEST_ASSERT_MBEDTLS_OK(mbedtls_mpi_write_binary(&s, sign.sign_s, key_len));

    TEST_ESP_OK(verify_ecdsa_sign(sha, sizeof(sha), &pubkey, &sign, is_crv_p192));

    mbedtls_pk_free(&key_ctx);
    mbedtls_ecdsa_free(&ecdsa_context);
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
}

TEST_CASE("Test TEE Secure Storage - mbedtls ECDSA signing", "[mbedtls]")
{
    test_ecdsa_sign(MBEDTLS_ECP_DP_SECP256R1, sha, MIN_SEC_STG_SLOT_ID);
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP192R1_SIGN
    test_ecdsa_sign(MBEDTLS_ECP_DP_SECP192R1, sha, MAX_SEC_STG_SLOT_ID);
#endif
}
#endif
