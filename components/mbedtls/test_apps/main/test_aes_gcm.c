/* mbedTLS GCM test
 *
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <esp_system.h>
#include "mbedtls/aes.h"
#include "mbedtls/gcm.h"
#include "unity.h"
#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "test_utils.h"
#include "ccomp_timer.h"
#include "sys/param.h"

#if CONFIG_MBEDTLS_HARDWARE_AES

/*
    Python example code for generating test vectors

    import os, binascii
    from cryptography.hazmat.primitives.ciphers.aead import AESGCM

    def as_c_array(byte_arr):
        hex_str = ''
        for idx, byte in enumerate(byte_arr):
            hex_str += "0x{:02x}, ".format(byte)
            bytes_per_line = 8
            if idx % bytes_per_line == bytes_per_line - 1:
                hex_str += '\n'

        return hex_str

    key = b'\x44' * 16
    iv = b'\xEE' * 16
    data = b'\xAA' * 3200
    aad = b'\x76' * 16

    aesgcm = AESGCM(key)

    ct = aesgcm.encrypt(iv, data, aad)

    print(as_c_array(ct))
*/

TEST_CASE("mbedtls GCM stream test", "[aes-gcm]")
{

    const unsigned SZ = 100;
    mbedtls_gcm_context ctx;
    uint8_t nonce[16];
    uint8_t key[16];
    uint8_t tag[16];
    mbedtls_cipher_id_t cipher = MBEDTLS_CIPHER_ID_AES;

    const uint8_t expected_cipher[] = {
        0x03, 0x92, 0x13, 0x49, 0x1f, 0x1f, 0x24, 0x41,
        0xe8, 0xeb, 0x89, 0x47, 0x50, 0x0a, 0xce, 0xa3,
        0xc7, 0x1c, 0x10, 0x70, 0xb0, 0x89, 0x82, 0x5e,
        0x0f, 0x4a, 0x23, 0xee, 0xd2, 0xfc, 0xff, 0x45,
        0x61, 0x4c, 0xd1, 0xfb, 0x6d, 0xe2, 0xbe, 0x67,
        0x6f, 0x94, 0x72, 0xa3, 0xe7, 0x04, 0x99, 0xb3,
        0x4a, 0x46, 0xf9, 0x2b, 0xaf, 0xac, 0xa9, 0x0e,
        0x43, 0x7e, 0x8b, 0xc4, 0xbf, 0x49, 0xa4, 0x83,
        0x9c, 0x31, 0x11, 0x1c, 0x09, 0xac, 0x90, 0xdf,
        0x00, 0x34, 0x08, 0xe5, 0x70, 0xa3, 0x7e, 0x4b,
        0x36, 0x48, 0x5a, 0x3f, 0x28, 0xc7, 0x1c, 0xd9,
        0x1b, 0x1b, 0x49, 0x96, 0xe9, 0x7c, 0xea, 0x54,
        0x7c, 0x71, 0x29, 0x0d
    };
    const uint8_t expected_tag[] = {
        0x35, 0x1c, 0x21, 0xc6, 0xbc, 0x6b, 0x18, 0x52,
        0x90, 0xe1, 0xf2, 0x5b, 0xe1, 0xf6, 0x15, 0xee,
    };


    memset(nonce, 0x89, 16);
    memset(key, 0x56, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0xAB, SZ);
    /* Test that all the end results are the same
        no matter how many bytes we encrypt each call
        */
    for (int bytes_to_process = 16; bytes_to_process < SZ; bytes_to_process = bytes_to_process + 16) {
        memset(nonce, 0x89, 16);
        memset(ciphertext, 0x0, SZ);
        memset(decryptedtext, 0x0, SZ);
        memset(tag, 0x0, 16);

        mbedtls_gcm_init(&ctx);
        mbedtls_gcm_setkey(&ctx, cipher, key, 128);
        mbedtls_gcm_starts( &ctx, MBEDTLS_AES_ENCRYPT, nonce, sizeof(nonce) );
        mbedtls_gcm_update_ad( &ctx, NULL, 0 );

        size_t olen;
        // Encrypt
        for (int idx = 0; idx < SZ; idx = idx + bytes_to_process) {
            // Limit length of last call to avoid exceeding buffer size
            size_t length = (idx + bytes_to_process > SZ) ? (SZ - idx) : bytes_to_process;
            mbedtls_gcm_update(&ctx, plaintext + idx, length, ciphertext + idx, length, &olen);
        }
        mbedtls_gcm_finish( &ctx, NULL, 0, &olen, tag, sizeof(tag) );
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher, ciphertext, SZ);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_tag, tag, sizeof(tag));

        // Decrypt
        memset(nonce, 0x89, 16);
        mbedtls_gcm_free( &ctx );

        mbedtls_gcm_init(&ctx);
        mbedtls_gcm_setkey(&ctx, cipher, key, 128);
        mbedtls_gcm_starts( &ctx, MBEDTLS_AES_DECRYPT, nonce, sizeof(nonce));
        mbedtls_gcm_update_ad( &ctx, NULL, 0 );

        for (int idx = 0; idx < SZ; idx = idx + bytes_to_process) {
            // Limit length of last call to avoid exceeding buffer size

            size_t length = (idx + bytes_to_process > SZ) ? (SZ - idx) : bytes_to_process;
            mbedtls_gcm_update(&ctx, ciphertext + idx, length, decryptedtext + idx, length, &olen);
        }
        mbedtls_gcm_finish( &ctx, NULL, 0, &olen, tag, sizeof(tag) );
        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);
        mbedtls_gcm_free( &ctx );
    }
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls AES GCM self-tests", "[aes-gcm]")
{
    TEST_ASSERT_FALSE_MESSAGE(mbedtls_gcm_self_test(1), "AES GCM self-test should pass.");
}

typedef struct  {
    uint8_t *plaintext;
    size_t plaintext_length;
    uint32_t output_caps;
    uint8_t *add_buf;
    size_t add_length;
    uint8_t *iv;
    size_t iv_length;
    uint8_t *key;
    size_t key_bits;
    size_t tag_len;
} aes_gcm_test_cfg_t;

typedef struct {
    const uint8_t *expected_tag;
    const uint8_t *ciphertext_last_block; // Last block of the ciphertext
} aes_gcm_test_expected_res_t;


typedef enum {
    AES_GCM_TEST_CRYPT_N_TAG,
    AES_GCM_TEST_START_UPDATE_FINISH,
} aes_gcm_test_type_t;

static void aes_gcm_test(aes_gcm_test_cfg_t *cfg, aes_gcm_test_expected_res_t *res, aes_gcm_test_type_t aes_gcm_type)
{
    mbedtls_cipher_id_t cipher = MBEDTLS_CIPHER_ID_AES;
    mbedtls_gcm_context ctx;

    uint8_t tag_buf_encrypt[16] = {};
    uint8_t tag_buf_decrypt[16] = {};
    uint8_t iv_buf[16] = {};

    uint8_t *ciphertext = heap_caps_malloc(cfg->plaintext_length, cfg->output_caps);
    uint8_t *output = heap_caps_malloc(cfg->plaintext_length, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

    if (cfg->plaintext_length != 0) {
        TEST_ASSERT_NOT_NULL(ciphertext);
        TEST_ASSERT_NOT_NULL(output);
    }

    memset(ciphertext, 0, cfg->plaintext_length);
    memset(output, 0, cfg->plaintext_length);
    memcpy(iv_buf, cfg->iv, cfg->iv_length);

    mbedtls_gcm_init(&ctx);
    mbedtls_gcm_setkey(&ctx, cipher, cfg->key, cfg->key_bits);
    size_t olen;
    /* Encrypt and tag */
    if (aes_gcm_type == AES_GCM_TEST_CRYPT_N_TAG) {
        mbedtls_gcm_crypt_and_tag(&ctx, MBEDTLS_AES_ENCRYPT, cfg->plaintext_length, iv_buf, cfg->iv_length, cfg->add_buf, cfg->add_length, cfg->plaintext, ciphertext, cfg->tag_len, tag_buf_encrypt);
    } else if (aes_gcm_type == AES_GCM_TEST_START_UPDATE_FINISH) {
        TEST_ASSERT(mbedtls_gcm_starts( &ctx, MBEDTLS_AES_ENCRYPT, iv_buf, cfg->iv_length) == 0 );
        TEST_ASSERT(mbedtls_gcm_update_ad( &ctx, cfg->add_buf, cfg->add_length) == 0 );
        TEST_ASSERT(mbedtls_gcm_update( &ctx, cfg->plaintext, cfg->plaintext_length, ciphertext, cfg->plaintext_length, &olen) == 0 );
        TEST_ASSERT(mbedtls_gcm_finish( &ctx, NULL, 0, &olen, tag_buf_encrypt, cfg->tag_len) == 0 );
    }
    size_t offset = cfg->plaintext_length > 16 ? cfg->plaintext_length - 16 : 0;
    /* Sanity check: make sure the last ciphertext block matches what we expect to see. */
    TEST_ASSERT_EQUAL_HEX8_ARRAY(res->ciphertext_last_block, ciphertext + offset, MIN(16, cfg->plaintext_length));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(res->expected_tag, tag_buf_encrypt, cfg->tag_len);


    /* Decrypt and authenticate */
    if (aes_gcm_type == AES_GCM_TEST_CRYPT_N_TAG) {
        TEST_ASSERT(mbedtls_gcm_auth_decrypt(&ctx, cfg->plaintext_length, iv_buf, cfg->iv_length, cfg->add_buf, cfg->add_length, res->expected_tag, cfg->tag_len, ciphertext, output) == 0);
    } else if (aes_gcm_type == AES_GCM_TEST_START_UPDATE_FINISH) {
        TEST_ASSERT(mbedtls_gcm_starts( &ctx, MBEDTLS_AES_DECRYPT, iv_buf, cfg->iv_length) == 0 );
        TEST_ASSERT(mbedtls_gcm_update_ad( &ctx, cfg->add_buf, cfg->add_length) == 0 );
        TEST_ASSERT(mbedtls_gcm_update( &ctx, ciphertext, cfg->plaintext_length, output, cfg->plaintext_length, &olen) == 0 );
        TEST_ASSERT(mbedtls_gcm_finish( &ctx, NULL, 0, &olen, tag_buf_decrypt, cfg->tag_len) == 0 );

        /* mbedtls_gcm_auth_decrypt already checks tag so only needed for AES_GCM_TEST_START_UPDATE_FINISH */
        TEST_ASSERT_EQUAL_HEX8_ARRAY(res->expected_tag, tag_buf_decrypt, cfg->tag_len);
    }

    TEST_ASSERT_EQUAL_HEX8_ARRAY(cfg->plaintext, output, cfg->plaintext_length);
    mbedtls_gcm_free( &ctx );
    free(ciphertext);
    free(output);
}



TEST_CASE("mbedtls AES GCM", "[aes-gcm]")
{
    uint8_t iv[16];
    uint8_t key[16];
    uint8_t add[30];

    memset(iv, 0xB1, sizeof(iv));
    memset(key, 0x27, sizeof(key));
    memset(add, 0x90, sizeof(add));

    size_t length[] = {10, 16, 500, 5000, 12345};

    const uint8_t expected_last_block[][16] = {

        {
            0x37, 0x99, 0x4b, 0x16, 0x5f, 0x8d, 0x27, 0xb1,
            0x60, 0x72
        },

        {
            0x37, 0x99, 0x4b, 0x16, 0x5f, 0x8d, 0x27, 0xb1,
            0x60, 0x72, 0x9a, 0x81, 0x8d, 0x3c, 0x69, 0x66
        },

        {
            0x9d, 0x7a, 0xac, 0x84, 0xe3, 0x70, 0x43, 0x0f,
            0xa7, 0x83, 0x43, 0xc9, 0x04, 0xf8, 0x7d, 0x48
        },

        {
            0xee, 0xfd, 0xab, 0x2a, 0x09, 0x44, 0x41, 0x6a,
            0x91, 0xb0, 0x74, 0x24, 0xee, 0x35, 0xb1, 0x39
        },

        {
            0x51, 0xf7, 0x1f, 0x67, 0x1a, 0x4a, 0x12, 0x37,
            0x60, 0x3b, 0x68, 0x01, 0x20, 0x4f, 0xf3, 0xd9
        },
    };

    const uint8_t expected_tag[][16] = {

        {
            0x06, 0x4f, 0xb5, 0x91, 0x12, 0x24, 0xb4, 0x24,
            0x0b, 0xc2, 0x85, 0x59, 0x6a, 0x7c, 0x1f, 0xc9
        },

        {
            0x45, 0xc2, 0xa8, 0xfe, 0xff, 0x49, 0x1f, 0x45,
            0x8e, 0x29, 0x74, 0x41, 0xed, 0x9b, 0x54, 0x28
        },

        {
            0xe1, 0xf9, 0x40, 0xfa, 0x29, 0x6f, 0x30, 0xae,
            0xb6, 0x9b, 0x33, 0xdb, 0x8a, 0xf9, 0x70, 0xc4
        },

        {
            0x22, 0xe1, 0x22, 0x34, 0x0c, 0x91, 0x0b, 0xcf,
            0xa3, 0x42, 0xe0, 0x48, 0xe6, 0xfe, 0x2e, 0x28
        },

        {
            0xfb, 0xfe, 0x5a, 0xed, 0x26, 0x5c, 0x5e, 0x66,
            0x4e, 0xb2, 0x48, 0xce, 0xe9, 0x88, 0x1c, 0xe0
        },
    };

    aes_gcm_test_cfg_t cfg = {
        .output_caps = MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL,
        .iv = iv,
        .iv_length = sizeof(iv),
        .key = key,
        .key_bits = 8 * sizeof(key),
        .add_buf = add,
        .add_length = sizeof(add),
        .tag_len = 16
    };

    aes_gcm_test_expected_res_t res = {
    };

    for (int i = 0; i < sizeof(length) / sizeof(length[0]); i++) {
        printf("Test AES-GCM with plaintext length = %d\n", length[i]);
        uint8_t *input = heap_caps_malloc(length[i], MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
        TEST_ASSERT(input != NULL || length[i] == 0);
        memset(input, 0x36, length[i]);

        cfg.plaintext = input;
        cfg.plaintext_length = length[i];
        res.expected_tag = expected_tag[i];
        res.ciphertext_last_block = expected_last_block[i],

        aes_gcm_test(&cfg, &res, AES_GCM_TEST_CRYPT_N_TAG);
        aes_gcm_test(&cfg, &res, AES_GCM_TEST_START_UPDATE_FINISH);

        free(input);
    }
}


TEST_CASE("mbedtls AES GCM - Different add messages", "[aes-gcm]")
{
    const unsigned CALL_SZ = 160;
    uint8_t iv[16];
    uint8_t key[16];
    uint8_t *input = heap_caps_malloc(CALL_SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(input);

    memset(input, 0x67, CALL_SZ);
    memset(iv, 0xA2, sizeof(iv));
    memset(key, 0x48, sizeof(key));

    const uint8_t expected_last_block[] = {
        0xcd, 0xb9, 0xad, 0x6f, 0xc9, 0x35, 0x21, 0x0d,
        0xc9, 0x5d, 0xea, 0xd9, 0xf7, 0x1d, 0x43, 0xed
    };

    size_t add_len[] = {0, 10, 16, 500, 5000};

    const uint8_t expected_tag[][16] = {
        {
            0xe3, 0x91, 0xad, 0x40, 0x96, 0xb7, 0x8c, 0x53,
            0x4d, 0x15, 0x7d, 0x55, 0x15, 0xdf, 0x10, 0x69
        },

        {
            0xc2, 0x38, 0x36, 0xe9, 0x12, 0x72, 0x5b, 0x31,
            0x0c, 0xde, 0xb5, 0xc9, 0x8c, 0xa3, 0xcb, 0xe7
        },

        {
            0x57, 0x10, 0x22, 0x91, 0x65, 0xfa, 0x89, 0xba,
            0x0a, 0x3e, 0xc1, 0x7c, 0x93, 0x6e, 0x35, 0xac
        },

        {
            0x3c, 0x28, 0x03, 0xc2, 0x14, 0x40, 0xec, 0xb6,
            0x25, 0xfb, 0xdd, 0x55, 0xa0, 0xb2, 0x47, 0x7b
        },

        {
            0xfa, 0x66, 0x4a, 0x97, 0x2d, 0x02, 0x32, 0x5b,
            0x92, 0x94, 0xf1, 0x00, 0x1c, 0xfa, 0xe3, 0x07
        }
    };

    aes_gcm_test_cfg_t cfg = {
        .plaintext = input,
        .plaintext_length = CALL_SZ,
        .output_caps = MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL,
        .iv = iv,
        .iv_length = sizeof(iv),
        .key = key,
        .key_bits = 8 * sizeof(key),
        .tag_len = 16
    };

    aes_gcm_test_expected_res_t res = {
        .ciphertext_last_block = expected_last_block,
    };

    for (int i = 0; i < sizeof(add_len) / sizeof(add_len[0]); i++) {
        printf("Test AES-GCM with add length = %d\n", add_len[i]);
        uint8_t *add = heap_caps_malloc(add_len[i], MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
        TEST_ASSERT(add != NULL || add_len[i] == 0);
        memset(add, 0x12, add_len[i]);

        cfg.add_buf = add;
        cfg.add_length = add_len[i];
        res.expected_tag = expected_tag[i];

        aes_gcm_test(&cfg, &res, AES_GCM_TEST_CRYPT_N_TAG);
        aes_gcm_test(&cfg, &res, AES_GCM_TEST_START_UPDATE_FINISH);

        free(add);
    }
    free(input);
}



TEST_CASE("mbedtls AES GCM performance, start, update, ret", "[aes-gcm]")
{
    const unsigned CALL_SZ = 16 * 3200;
    mbedtls_gcm_context ctx;
    float elapsed_usec;
    unsigned char tag_buf[16];
    mbedtls_cipher_id_t cipher = MBEDTLS_CIPHER_ID_AES;
    uint8_t iv[16];
    uint8_t key[16];
    uint8_t aad[16];
    size_t olen;
    memset(iv, 0xEE, 16);
    memset(key, 0x44, 16);
    memset(aad, 0x76, 16);

    // allocate internal memory
    uint8_t *buf = heap_caps_malloc(CALL_SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buf);

    mbedtls_gcm_init(&ctx);
    mbedtls_gcm_setkey( &ctx, cipher, key, 128);

    ccomp_timer_start();

    memset(buf, 0xAA, CALL_SZ);

    TEST_ASSERT(mbedtls_gcm_starts( &ctx, MBEDTLS_AES_ENCRYPT, iv, sizeof(iv) ) == 0 );
    TEST_ASSERT(mbedtls_gcm_update_ad( &ctx, aad, sizeof(aad)) == 0 );
    TEST_ASSERT(mbedtls_gcm_update( &ctx, buf, CALL_SZ, buf, CALL_SZ, &olen) == 0 );
    TEST_ASSERT(mbedtls_gcm_finish( &ctx, NULL, 0, &olen, tag_buf, 16 ) == 0 );

    elapsed_usec = ccomp_timer_stop();

    /* Sanity check: make sure the last ciphertext block matches
       what we expect to see.
    */
    const uint8_t expected_last_block[] = {
        0xd4, 0x25, 0x88, 0xd4, 0x32, 0x52, 0x3d, 0x6f,
        0xae, 0x49, 0x19, 0xb5, 0x95, 0x01, 0xde, 0x7d,
    };

    const uint8_t expected_tag[] = {
        0xf5, 0x10, 0x1f, 0x21, 0x5b, 0x07, 0x0d, 0x3f,
        0xac, 0xc9, 0xd0, 0x42, 0x45, 0xef, 0xc7, 0xfa,
    };

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_last_block, buf + CALL_SZ - 16, 16);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_tag, tag_buf, 16);

    free(buf);

    // bytes/usec = MB/sec
    float mb_sec = CALL_SZ / elapsed_usec;
    printf("GCM encryption rate %.3fMB/sec\n", mb_sec);

#ifdef CONFIG_MBEDTLS_HARDWARE_GCM
    // Don't put a hard limit on software AES performance
    TEST_PERFORMANCE_GREATER_THAN(AES_GCM_UPDATE_THROUGHPUT_MBSEC, "%.3fMB/sec", mb_sec);
#endif
}


TEST_CASE("mbedtls AES GCM performance, crypt-and-tag", "[aes-gcm]")
{
    const unsigned CALL_SZ = 16 * 3200;
    mbedtls_gcm_context ctx;
    float elapsed_usec;
    unsigned char tag_buf[16] = {};
    mbedtls_cipher_id_t cipher = MBEDTLS_CIPHER_ID_AES;
    uint8_t iv[16];
    uint8_t key[16];
    uint8_t aad[16];

    memset(iv, 0xEE, 16);
    memset(key, 0x44, 16);
    memset(aad, 0x76, 16);

    // allocate internal memory
    uint8_t *buf = heap_caps_malloc(CALL_SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buf);

    mbedtls_gcm_init(&ctx);
    mbedtls_gcm_setkey( &ctx, cipher, key, 128);

    memset(buf, 0xAA, CALL_SZ);

    ccomp_timer_start();
    mbedtls_gcm_crypt_and_tag(&ctx, MBEDTLS_AES_ENCRYPT, CALL_SZ, iv, sizeof(iv), aad, sizeof(aad), buf, buf, 16, tag_buf);

    elapsed_usec = ccomp_timer_stop();

    /* Sanity check: make sure the last ciphertext block matches
       what we expect to see.
    */

    const uint8_t expected_last_block[] = {
        0xd4, 0x25, 0x88, 0xd4, 0x32, 0x52, 0x3d, 0x6f,
        0xae, 0x49, 0x19, 0xb5, 0x95, 0x01, 0xde, 0x7d,
    };

    const uint8_t expected_tag[] = {
        0xf5, 0x10, 0x1f, 0x21, 0x5b, 0x07, 0x0d, 0x3f,
        0xac, 0xc9, 0xd0, 0x42, 0x45, 0xef, 0xc7, 0xfa,
    };

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_last_block, buf + CALL_SZ - 16, 16);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_tag, tag_buf, 16);

    free(buf);

    // bytes/usec = MB/sec
    float mb_sec = CALL_SZ / elapsed_usec;
    printf("GCM encryption rate %.3fMB/sec\n", mb_sec);

#ifdef CONFIG_MBEDTLS_HARDWARE_GCM
    // Don't put a hard limit on software AES performance
    TEST_PERFORMANCE_GREATER_THAN(AES_GCM_CRYPT_TAG_THROUGHPUT_MBSEC, "%.3fMB/sec", mb_sec);
#endif
}

TEST_CASE("mbedtls AES GCM - Combine different IV/Key/Plaintext/AAD lengths", "[aes-gcm]")
{
    #define IV_BYTES_VALUE 0xA2
    #define KEY_BYTES_VALUE 0x48
    #define INPUT_BYTES_VALUE 0x36
    #define ADD_BYTES_VALUE 0x12

    uint8_t iv[16];
    uint8_t key[32];

    memset(iv, IV_BYTES_VALUE, sizeof(iv));
    memset(key, KEY_BYTES_VALUE, sizeof(key));

    /* Key length is: 16 bytes, 32 bytes */
    size_t key_length[] = {16, 32};

    /* IV length is: 12 bytes (standard), 16 bytes */
    size_t iv_length[] = {12, 16};

    /* Plaintext length is: a multiple of 16 bytes, a non-multiple of 16 bytes */
    size_t length[] = {160, 321};

    /* Add len is: 0, a multiple of 16 bytes, a non-multiple of 16 bytes */
    size_t add_len[] = {0, 160, 321};

    /*indexes: Key - IV - Plaintext */
    const uint8_t expected_last_block[2][2][2][16] = {
        {
            /* 16 byte key */

            {
                {
                    0xa2, 0x1e, 0x23, 0x3c, 0xfc, 0x7c, 0xec, 0x9a,
                    0x91, 0xe5, 0xdb, 0x3a, 0xe5, 0x0c, 0x3f, 0xc2,
                },

                {
                    0xa8, 0xeb, 0x40, 0x9b, 0x7b, 0x87, 0x07,
                    0x68, 0x17, 0x5c, 0xc0, 0xb7, 0xb4, 0xb3, 0x81,
                    0xbe,
                }
            },
            {
                {
                    0x9c, 0xe8, 0xfc, 0x3e, 0x98, 0x64, 0x70, 0x5c,
                    0x98, 0x0c, 0xbb, 0x88, 0xa6, 0x4c, 0x12, 0xbc
                },

                {
                    0x8b, 0x66, 0xf5, 0xbc, 0x56, 0x59, 0xae,
                    0xf0, 0x9e, 0x5c, 0xdb, 0x6d, 0xfc, 0x1f, 0x2e,
                    0x00
                }
            },
        },
        {
            /* 32 byte key */
            {
                {
                    0xde, 0xc2, 0xd3, 0xeb, 0x5e, 0x03, 0x53, 0x4b,
                    0x04, 0x0d, 0x63, 0xf1, 0xd8, 0x5b, 0x1f, 0x85,
                },

                {
                    0xb5, 0x53, 0x8e, 0xd3, 0xab, 0x10, 0xf1,
                    0x77, 0x41, 0x92, 0xea, 0xdd, 0xdd, 0x9e, 0x5d,
                    0x40,
                }
            },
            {
                {
                    0x3b, 0xc7, 0xf0, 0x3f, 0xba, 0x97, 0xbd, 0xa0,
                    0xa5, 0x48, 0xf3, 0x7a, 0xde, 0x23, 0x19, 0x7a,
                },

                {
                    0x57, 0xc7, 0x4d, 0xe3, 0x79, 0x5e, 0xbd,
                    0x0d, 0xd7, 0x6a, 0xef, 0x1f, 0x54, 0x29, 0xa6,
                    0xd7,
                }
            },
        },
    };

    /*indexes: Key - IV - Plaintext - Add len*/
    const uint8_t expected_tag[2][2][2][3][16] = {
        {
            {
                {
                    // Plaintext 160 bytes
                    {
                        0x67, 0x92, 0xb1, 0x7f, 0x44, 0x1f, 0x95, 0xfb,
                        0x33, 0x76, 0x66, 0xb7, 0x4f, 0x3e, 0xec, 0x4d,
                    },

                    {
                        0xb1, 0x99, 0xed, 0x1b, 0x4e, 0x12, 0x87, 0x5e,
                        0xf4, 0xe3, 0x81, 0xd8, 0x96, 0x07, 0xda, 0xff,
                    },

                    {
                        0x73, 0x35, 0x0c, 0xf5, 0x70, 0x1e, 0xc0, 0x99,
                        0x34, 0xba, 0x1a, 0x50, 0x23, 0xac, 0x21, 0x33,
                    },
                },
                {
                    // Plaintext 321 bytes
                    {
                        0x2d, 0xf6, 0xd0, 0x7a, 0x75, 0x4d, 0x9d,
                        0xb5, 0x9d, 0x43, 0xbf, 0x57, 0x10, 0xa3, 0xff,
                        0x3d
                    },

                    {
                        0x06, 0x91, 0xe4, 0x38, 0x3a, 0xe1, 0x6e,
                        0x2d, 0x83, 0x68, 0x2e, 0xb0, 0x26, 0x2f, 0xe4,
                        0x78
                    },

                    {
                        0x1b, 0x58, 0x2f, 0x9b, 0xe9, 0xe0, 0xe0,
                        0x43, 0x83, 0x08, 0xec, 0x58, 0x3a, 0x78, 0xe9,
                        0x69,
                    }
                }
            },
            {
                {
                    // Plaintext 160 bytes
                    {
                        0x77, 0xe5, 0x2e, 0x2d, 0x94, 0xb8, 0x03, 0x61,
                        0x7a, 0xd5, 0x0c, 0x3c, 0x9c, 0x40, 0x92, 0x9b
                    },

                    {
                        0xa1, 0xee, 0x72, 0x49, 0x9e, 0xb5, 0x11, 0xc4,
                        0xbd, 0x40, 0xeb, 0x53, 0x45, 0x79, 0xa4, 0x29
                    },

                    {
                        0x63, 0x42, 0x93, 0xa7, 0xa0, 0xb9, 0x56, 0x03,
                        0x7d, 0x19, 0x70, 0xdb, 0xf0, 0xd2, 0x5f, 0xe5
                    },
                },
                {
                    // Plaintext 321 bytes
                    {
                        0x50, 0xa3, 0x79, 0xfc, 0x17, 0xb8, 0xf4,
                        0xf6, 0x14, 0xaa, 0x4a, 0xe7, 0xd4, 0xa0, 0xea,
                        0xee
                    },

                    {
                        0x7b, 0xc4, 0x4d, 0xbe, 0x58, 0x14, 0x07,
                        0x6e, 0x0a, 0x81, 0xdb, 0x00, 0xe2, 0x2c, 0xf1,
                        0xab
                    },

                    {
                        0x66, 0x0d, 0x86, 0x1d, 0x8b, 0x15, 0x89,
                        0x00, 0x0a, 0xe1, 0x19, 0xe8, 0xfe, 0x7b, 0xfc,
                        0xba
                    }
                }
            },
        },
        {
            {
                {
                    // Plaintext 160 bytes
                    {
                        0x04, 0x04, 0x15, 0xb1, 0xd3, 0x98, 0x15, 0x45,
                        0xa2, 0x44, 0xba, 0x4a, 0xde, 0xc2, 0x8d, 0xd6,
                    },

                    {
                        0x94, 0x3e, 0xc3, 0x5d, 0xdc, 0x42, 0xf6, 0x4c,
                        0x80, 0x15, 0xe4, 0xb9, 0x0b, 0xc9, 0x87, 0x01,
                    },

                    {
                        0x93, 0x6e, 0x26, 0x5b, 0x7e, 0x17, 0xc8, 0x73,
                        0x9b, 0x71, 0x31, 0x7a, 0x8b, 0x0e, 0x19, 0x89,
                    }
                },
                {
                    // Plaintext 321 bytes
                    {
                        0x99, 0x5e, 0x77, 0x28, 0x8b, 0xa8, 0x9b,
                        0xb3, 0x35, 0xc3, 0x99, 0x90, 0xd4, 0x5d, 0x63,
                        0xa7,
                    },

                    {
                        0xbc, 0xc2, 0x9f, 0xe6, 0x38, 0xef, 0xf5,
                        0x11, 0x76, 0x09, 0x17, 0x3a, 0xd4, 0x91, 0xee,
                        0xfe,
                    },

                    {
                        0x9f, 0xa6, 0x23, 0x5a, 0x4d, 0x78, 0xae,
                        0xce, 0x10, 0x35, 0xc1, 0x0c, 0x6e, 0xc2, 0x4e,
                        0xe8,
                    }
                }
            },
            {
                {
                    // Plaintext 160 bytes
                    {
                        0xfb, 0x74, 0x7e, 0x21, 0xf2, 0xe7, 0xe3, 0xf5,
                        0xfa, 0xc8, 0x23, 0xab, 0x54, 0x9a, 0xb9, 0xcf,
                    },

                    {
                        0x6b, 0x4e, 0xa8, 0xcd, 0xfd, 0x3d, 0x00, 0xfc,
                        0xd8, 0x99, 0x7d, 0x58, 0x81, 0x91, 0xb3, 0x18,
                    },

                    {
                        0x6c, 0x1e, 0x4d, 0xcb, 0x5f, 0x68, 0x3e, 0xc3,
                        0xc3, 0xfd, 0xa8, 0x9b, 0x01, 0x56, 0x2d, 0x90,
                    },
                },
                {
                    // Plaintext 321 bytes
                    {
                        0xcd, 0x49, 0x75, 0x4c, 0x2a, 0x62, 0x65,
                        0x6f, 0xfe, 0x14, 0xc2, 0x5d, 0x41, 0x07, 0x24,
                        0x55
                    },

                    {
                        0xe8, 0xd5, 0x9d, 0x82, 0x99, 0x25, 0x0b,
                        0xcd, 0xbd, 0xde, 0x4c, 0xf7, 0x41, 0xcb, 0xa9,
                        0x0c,
                    },

                    {
                        0xcb, 0xb1, 0x21, 0x3e, 0xec, 0xb2, 0x50,
                        0x12, 0xdb, 0xe2, 0x9a, 0xc1, 0xfb, 0x98, 0x09,
                        0x1a,
                    }
                }
            },
        },
    };

    aes_gcm_test_cfg_t cfg = {
        .output_caps = MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL,
        .tag_len = 16
    };


    for (int i_key = 0; i_key < sizeof(key_length) / sizeof(key_length[0]); i_key++) {
        printf("Test AES-GCM with key length = %d\n",  key_length[i_key]);

        cfg.key = key;
        cfg.key_bits = 8 * key_length[i_key];

        for (int i_iv = 0; i_iv < sizeof(iv_length) / sizeof(iv_length[0]); i_iv++) {
            printf("Test AES-GCM with IV length = %d\n",  iv_length[i_iv]);

            cfg.iv = iv;
            cfg.iv_length = iv_length[i_iv];

            for (int i_len = 0; i_len < sizeof(length) / sizeof(length[0]); i_len++) {
                printf("Test AES-GCM with plaintext length = %d\n", length[i_len]);
                uint8_t *input = heap_caps_malloc(length[i_len], MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
                TEST_ASSERT(input != NULL || length[i_len] == 0);
                memset(input, INPUT_BYTES_VALUE, length[i_len]);
                cfg.plaintext = input;
                cfg.plaintext_length = length[i_len];

                aes_gcm_test_expected_res_t res = {0};

                for (int i_add = 0; i_add < sizeof(add_len) / sizeof(add_len[0]); i_add++) {

                    printf("Test AES-GCM with add length = %d\n", add_len[i_add]);
                    uint8_t *add = heap_caps_malloc(add_len[i_add], MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
                    TEST_ASSERT(add != NULL || add_len[i_add] == 0);
                    memset(add, ADD_BYTES_VALUE, add_len[i_add]);

                    cfg.add_buf = add;
                    cfg.add_length = add_len[i_add];

                    res.expected_tag = expected_tag[i_key][i_iv][i_len][i_add];
                    res.ciphertext_last_block = expected_last_block[i_key][i_iv][i_len],

                    aes_gcm_test(&cfg, &res, AES_GCM_TEST_CRYPT_N_TAG);

                    free(add);
                }
                free(input);
            }
        }
    }
}

TEST_CASE("mbedtls AES GCM - Different Authentication Tag lengths", "[aes-gcm]")
{
    const unsigned CALL_SZ = 160;
    uint8_t iv[16];
    uint8_t key[16];
    uint8_t aad[16];
    uint8_t *input = heap_caps_malloc(CALL_SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(input);

    memset(input, 0x67, CALL_SZ);
    memset(iv, 0xA2, sizeof(iv));
    memset(key, 0x48, sizeof(key));
    memset(aad, 0x12, sizeof(aad));

    size_t tag_len[] = {4, 8, 11, 16};

    const uint8_t expected_last_block[] = {
        0xcd, 0xb9, 0xad, 0x6f, 0xc9, 0x35, 0x21, 0x0d,
        0xc9, 0x5d, 0xea, 0xd9, 0xf7, 0x1d, 0x43, 0xed
    };

    const uint8_t expected_tag[16] = {
        0x57, 0x10, 0x22, 0x91, 0x65, 0xfa, 0x89, 0xba,
        0x0a, 0x3e, 0xc1, 0x7c, 0x93, 0x6e, 0x35, 0xac
    };

    aes_gcm_test_cfg_t cfg = {
        .plaintext = input,
        .plaintext_length = CALL_SZ,
        .output_caps = MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL,
        .add_buf = aad,
        .add_length = sizeof(aad),
        .iv = iv,
        .iv_length = sizeof(iv),
        .key = key,
        .key_bits = 8 * sizeof(key),
    };

    aes_gcm_test_expected_res_t res = {
        .expected_tag = expected_tag,
        .ciphertext_last_block = expected_last_block,
    };

    for (int i = 0; i < sizeof(tag_len) / sizeof(tag_len[0]); i++) {
        printf("Test AES-GCM with tag length = %d\n", tag_len[i]);
        cfg.tag_len = tag_len[i];
        aes_gcm_test(&cfg, &res, AES_GCM_TEST_CRYPT_N_TAG);
        aes_gcm_test(&cfg, &res, AES_GCM_TEST_START_UPDATE_FINISH);
    }
    free(input);
}

#endif //CONFIG_MBEDTLS_HARDWARE_AES
