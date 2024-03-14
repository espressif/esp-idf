/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include "sys/param.h"
#include "esp_heap_caps.h"
#include "mbedtls/gcm.h"
#include "sdkconfig.h"
#include "unity.h"


#if CONFIG_MBEDTLS_GCM_SUPPORT_NON_AES_CIPHER

typedef struct  {
    uint8_t *plaintext;
    size_t plaintext_length;
    uint8_t *aad_buf;
    size_t aad_length;
    uint8_t *iv;
    size_t iv_length;
    uint8_t *key;
    size_t key_bits;
    size_t tag_len;
} gcm_test_cfg_t;

typedef struct {
    const uint8_t *expected_tag;
    const uint8_t *ciphertext_last_block; // Last block of the ciphertext
} gcm_test_expected_res_t;

typedef enum {
    GCM_TEST_CRYPT_N_TAG,
    GCM_TEST_START_UPDATE_FINISH,
} gcm_test_type_t;

static void gcm_test(gcm_test_cfg_t *cfg, gcm_test_expected_res_t *res, gcm_test_type_t gcm_type)
{
    mbedtls_gcm_context ctx;
    mbedtls_cipher_id_t cipher = MBEDTLS_CIPHER_ID_ARIA;

    uint8_t tag_buf_encrypt[16] = {};
    uint8_t tag_buf_decrypt[16] = {};
    uint8_t iv_buf[16] = {};
    uint8_t *ciphertext = malloc(cfg->plaintext_length);
    uint8_t *output = malloc(cfg->plaintext_length);
    size_t olen;

    if (cfg->plaintext_length != 0) {
        TEST_ASSERT_NOT_NULL(ciphertext);
        TEST_ASSERT_NOT_NULL(output);
    }

    memset(ciphertext, 0, cfg->plaintext_length);
    memset(output, 0, cfg->plaintext_length);
    memcpy(iv_buf, cfg->iv, cfg->iv_length);

    mbedtls_gcm_init(&ctx);
    TEST_ASSERT(mbedtls_gcm_setkey(&ctx, cipher, cfg->key, cfg->key_bits) == 0);

    if (gcm_type == GCM_TEST_CRYPT_N_TAG) {
        mbedtls_gcm_crypt_and_tag(&ctx, MBEDTLS_GCM_ENCRYPT, cfg->plaintext_length, iv_buf, cfg->iv_length, cfg->aad_buf, cfg->aad_length, cfg->plaintext, ciphertext, cfg->tag_len, tag_buf_encrypt);
    } else if (gcm_type == GCM_TEST_START_UPDATE_FINISH) {
        TEST_ASSERT(mbedtls_gcm_starts(&ctx, MBEDTLS_GCM_ENCRYPT, iv_buf, cfg->iv_length) == 0);
        TEST_ASSERT(mbedtls_gcm_update_ad(&ctx, cfg->aad_buf, cfg->aad_length) == 0);
        TEST_ASSERT(mbedtls_gcm_update(&ctx, cfg->plaintext, cfg->plaintext_length, ciphertext, cfg->plaintext_length, &olen) == 0);
        TEST_ASSERT(mbedtls_gcm_finish(&ctx, ciphertext, cfg->plaintext_length, &olen, tag_buf_encrypt, cfg->tag_len) == 0);
    }

    size_t offset = cfg->plaintext_length > 16 ? cfg->plaintext_length - 16 : 0;
    /* Sanity check: make sure the last ciphertext block matches what we expect to see. */
    TEST_ASSERT_EQUAL_HEX8_ARRAY(res->ciphertext_last_block, ciphertext + offset, MIN(16, cfg->plaintext_length));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(res->expected_tag, tag_buf_encrypt, cfg->tag_len);


    if (gcm_type == GCM_TEST_CRYPT_N_TAG) {
        TEST_ASSERT(mbedtls_gcm_auth_decrypt(&ctx, cfg->plaintext_length, iv_buf, cfg->iv_length, cfg->aad_buf, cfg->aad_length, res->expected_tag, cfg->tag_len, ciphertext, output) == 0);
    } else if (gcm_type == GCM_TEST_START_UPDATE_FINISH) {
        TEST_ASSERT(mbedtls_gcm_starts(&ctx, MBEDTLS_GCM_DECRYPT, iv_buf, cfg->iv_length) == 0);
        TEST_ASSERT(mbedtls_gcm_update_ad(&ctx, cfg->aad_buf, cfg->aad_length) == 0);
        TEST_ASSERT(mbedtls_gcm_update(&ctx, ciphertext, cfg->plaintext_length, output, cfg->plaintext_length, &olen) == 0);
        TEST_ASSERT(mbedtls_gcm_finish(&ctx, output, cfg->plaintext_length, &olen, tag_buf_decrypt, cfg->tag_len) == 0);

        /* mbedtls_gcm_auth_decrypt already checks tag so only needed for GCM_TEST_START_UPDATE_FINISH */
        TEST_ASSERT_EQUAL_HEX8_ARRAY(res->expected_tag, tag_buf_decrypt, cfg->tag_len);
    }

    TEST_ASSERT_EQUAL_HEX8_ARRAY(cfg->plaintext, output, cfg->plaintext_length);

    mbedtls_gcm_free(&ctx);
    free(ciphertext);
    free(output);
}


TEST_CASE("mbedtls ARIA GCM test", "[gcm]")
{
    const unsigned SZ = 1600;
    uint8_t aad[16];
    uint8_t iv[16];
    uint8_t key[16];

    const uint8_t expected_last_block[] = {
        0xbe, 0x96, 0xf1, 0x57, 0x34, 0x07, 0x3f, 0x9d,
        0x87, 0x6b, 0x39, 0x22, 0xe4, 0xef, 0xff, 0xf0,
    };
    const uint8_t expected_tag[] = {
        0xef, 0x4e, 0xa8, 0x24, 0x07, 0x65, 0x36, 0x12,
        0xb1, 0xde, 0x7e, 0x23, 0xda, 0xea, 0x7c, 0x6b,
    };

    uint8_t *plaintext = malloc(SZ);
    TEST_ASSERT_NOT_NULL(plaintext);

    memset(plaintext, 0xAA, SZ);
    memset(iv, 0xEE, 16);
    memset(key, 0x44, 16);
    memset(aad, 0x76, 16);

    gcm_test_cfg_t cfg = {
        .plaintext = plaintext,
        .plaintext_length = SZ,
        .iv = iv,
        .iv_length = sizeof(iv),
        .key = key,
        .key_bits = 8 * sizeof(key),
        .aad_buf = aad,
        .aad_length = sizeof(aad),
        .tag_len = 16
    };

    gcm_test_expected_res_t res = {
        .expected_tag = expected_tag,
        .ciphertext_last_block = expected_last_block,
    };

    gcm_test(&cfg, &res, GCM_TEST_CRYPT_N_TAG);
    gcm_test(&cfg, &res, GCM_TEST_START_UPDATE_FINISH);
    free(plaintext);
}

#endif /* CONFIG_MBEDTLS_GCM_SUPPORT_NON_AES_CIPHER */
