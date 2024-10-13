/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include "soc/soc_caps.h"
#include "esp_heap_caps.h"
#include "unity.h"
#include "test_params.h"
#include "memory_checks.h"
#include "unity_fixture.h"
#include "esp_log.h"
#include "aes/esp_aes.h"
#include "aes/esp_aes_gcm.h"

#if SOC_AES_SUPPORTED

#include "aes_block.h"

#define AES_BUFFER_SIZE 1600
#define AES_LONG_BUFFER_SIZE 8000

TEST_GROUP(aes);

TEST_SETUP(aes)
{
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(400, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(aes)
{
    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

static void test_cbc_aes(bool is_dma, size_t buffer_size, const uint8_t expected_cipher_end[32])
{
    esp_aes_context ctx;
    unsigned int key_bits = 256;
    uint8_t nonce[16];

    esp_aes_init(&ctx);
    esp_aes_setkey(&ctx, key_256, key_bits);

    uint8_t *chipertext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(chipertext);
    uint8_t *plaintext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(plaintext);
    uint8_t *decryptedtext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, buffer_size);
    memset(decryptedtext, 0x0, buffer_size);

    // Encrypt
    memcpy(nonce, iv, 16);
#ifdef SOC_AES_SUPPORT_DMA
    if (is_dma) {
        esp_aes_crypt_cbc(&ctx, ESP_AES_ENCRYPT, buffer_size, nonce, plaintext, chipertext);
    }
    else
#endif
    {
        aes_crypt_cbc_block(ESP_AES_ENCRYPT, key_bits / 8, key_256, buffer_size, nonce, plaintext, chipertext);
    }
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_end, chipertext + buffer_size - 32, 32);

    // Decrypt
    memcpy(nonce, iv, 16);
#ifdef SOC_AES_SUPPORT_DMA
    if (is_dma) {
        esp_aes_crypt_cbc(&ctx, ESP_AES_DECRYPT, buffer_size, nonce, chipertext, decryptedtext);
    }
    else
#endif
    {
        aes_crypt_cbc_block(ESP_AES_DECRYPT, key_bits / 8, key_256, buffer_size, nonce, chipertext, decryptedtext);
    }
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, buffer_size);

    esp_aes_free(&ctx);

    // Free dynamically allocated memory
    heap_caps_free(chipertext);
    heap_caps_free(plaintext);
    heap_caps_free(decryptedtext);
}

static void test_ctr_aes(bool is_dma, size_t buffer_size, const uint8_t expected_cipher_end[32])
{

    esp_aes_context ctx;
    unsigned int key_bits = 256;
    uint8_t nonce[16];
    uint8_t stream_block[16];
    size_t nc_off = 0;

    esp_aes_init(&ctx);
    esp_aes_setkey(&ctx, key_256, key_bits);

    uint8_t *chipertext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(chipertext);
    uint8_t *plaintext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(plaintext);
    uint8_t *decryptedtext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, buffer_size);
    memset(decryptedtext, 0x0, buffer_size);

    // Encrypt
    memcpy(nonce, iv, 16);
#ifdef SOC_AES_SUPPORT_DMA
    if (is_dma) {
        esp_aes_crypt_ctr(&ctx, buffer_size, &nc_off, nonce, stream_block, plaintext, chipertext);
    }
    else
#endif
    {
        aes_crypt_ctr_block(key_bits / 8, key_256, buffer_size, &nc_off, nonce, stream_block, plaintext, chipertext);
    }
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_end, chipertext + buffer_size - 32, 32);

    // Decrypt
    memcpy(nonce, iv, 16);
    nc_off = 0;
#ifdef SOC_AES_SUPPORT_DMA
    if (is_dma) {
        esp_aes_crypt_ctr(&ctx, buffer_size, &nc_off, nonce, stream_block, chipertext, decryptedtext);
    }
    else
#endif
    {
        aes_crypt_ctr_block(key_bits / 8, key_256, buffer_size, &nc_off, nonce, stream_block, chipertext, decryptedtext);
    }
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, buffer_size);

    esp_aes_free(&ctx);

    // Free dynamically allocated memory
    heap_caps_free(chipertext);
    heap_caps_free(plaintext);
    heap_caps_free(decryptedtext);
}

#ifdef SOC_AES_SUPPORT_DMA

static void test_ofb_aes(size_t buffer_size, const uint8_t expected_cipher_end[32])
{
    esp_aes_context ctx;
    unsigned int key_bits = 256;
    uint8_t nonce[16];
    size_t nc_off = 0;

    esp_aes_init(&ctx);
    esp_aes_setkey(&ctx, key_256, key_bits);

    uint8_t *chipertext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(chipertext);
    uint8_t *plaintext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(plaintext);
    uint8_t *decryptedtext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, buffer_size);
    memset(decryptedtext, 0x0, buffer_size);

    // Encrypt
    memcpy(nonce, iv, 16);
    esp_aes_crypt_ofb(&ctx, buffer_size, &nc_off, nonce, plaintext, chipertext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_end, chipertext + buffer_size - 32, 32);

    // Decrypt
    memcpy(nonce, iv, 16);
    nc_off = 0;
    esp_aes_crypt_ofb(&ctx, buffer_size, &nc_off, nonce, chipertext, decryptedtext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, buffer_size);

    esp_aes_free(&ctx);

    // Free dynamically allocated memory
    heap_caps_free(chipertext);
    heap_caps_free(plaintext);
    heap_caps_free(decryptedtext);
}

static void test_cfb8_aes(size_t buffer_size, const uint8_t expected_cipher_end[32])
{
    esp_aes_context ctx;
    unsigned int key_bits = 256;
    uint8_t nonce[16];

    esp_aes_init(&ctx);
    esp_aes_setkey(&ctx, key_256, key_bits);

    uint8_t *chipertext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(chipertext);
    uint8_t *plaintext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(plaintext);
    uint8_t *decryptedtext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, buffer_size);
    memset(decryptedtext, 0x0, buffer_size);

    // Encrypt
    memcpy(nonce, iv, 16);
    esp_aes_crypt_cfb8(&ctx, ESP_AES_ENCRYPT, buffer_size, nonce, plaintext, chipertext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_end, chipertext + buffer_size - 32, 32);

    // Decrypt
    memcpy(nonce, iv, 16);
    esp_aes_crypt_cfb8(&ctx, ESP_AES_DECRYPT, buffer_size, nonce, chipertext, decryptedtext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, buffer_size);

    esp_aes_free(&ctx);

    // Free dynamically allocated memory
    heap_caps_free(chipertext);
    heap_caps_free(plaintext);
    heap_caps_free(decryptedtext);
}

static void test_cfb128_aes(size_t buffer_size, const uint8_t expected_cipher_end[32])
{
    esp_aes_context ctx;
    unsigned int key_bits = 256;
    uint8_t nonce[16];
    size_t nc_off = 0;

    esp_aes_init(&ctx);
    esp_aes_setkey(&ctx, key_256, key_bits);

    uint8_t *chipertext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(chipertext);
    uint8_t *plaintext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(plaintext);
    uint8_t *decryptedtext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, buffer_size);
    memset(decryptedtext, 0x0, buffer_size);

    // Encrypt
    memcpy(nonce, iv, 16);
    esp_aes_crypt_cfb128(&ctx, ESP_AES_ENCRYPT, buffer_size, &nc_off, nonce, plaintext, chipertext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_end, chipertext + buffer_size - 32, 32);

    // Decrypt
    nc_off = 0;
    memcpy(nonce, iv, 16);
    esp_aes_crypt_cfb128(&ctx, ESP_AES_DECRYPT, buffer_size, &nc_off, nonce, chipertext, decryptedtext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, buffer_size);

    esp_aes_free(&ctx);

    // Free dynamically allocated memory
    heap_caps_free(chipertext);
    heap_caps_free(plaintext);
    heap_caps_free(decryptedtext);
}

#define CIPHER_ID_AES 2

static void test_gcm_aes(size_t length, const uint8_t expected_last_block[16], const uint8_t expected_tag[16])
{
    uint8_t iv[16];
    uint8_t key[16];
    uint8_t add[30];
    size_t tag_len = 16;
    esp_gcm_context ctx;
    uint8_t iv_buf[16] = {};
    size_t iv_length = sizeof(iv);
    size_t add_length = sizeof(add);
    uint8_t tag_buf_encrypt[16] = {};

    uint8_t *plaintext = heap_caps_malloc(length, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(plaintext);
    uint8_t *ciphertext = heap_caps_malloc(length, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(ciphertext);
    uint8_t *decryptedtext = heap_caps_malloc(length, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(iv, 0xB1, iv_length);
    memset(key, 0x27, sizeof(key));
    memset(add, 0x90, add_length);
    memset(plaintext, 0x36, length);
    memset(ciphertext, 0, length);
    memset(decryptedtext, 0, length);
    memcpy(iv_buf, iv, iv_length);

    esp_aes_gcm_init(&ctx);
    TEST_ASSERT(esp_aes_gcm_setkey(&ctx, CIPHER_ID_AES, key, 8 * sizeof(key)) == 0);

    /* Encrypt and authenticate */
    TEST_ASSERT(esp_aes_gcm_crypt_and_tag(&ctx, ESP_AES_ENCRYPT, length, iv_buf, iv_length, add, add_length, plaintext, ciphertext, tag_len, tag_buf_encrypt) == 0);
    size_t offset = length > 16 ? length - 16 : 0;
    /* Sanity check: make sure the last ciphertext block matches what we expect to see. */
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_last_block, ciphertext + offset, MIN(16, length));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_tag, tag_buf_encrypt, tag_len);

    /* Decrypt and authenticate */
    TEST_ASSERT(esp_aes_gcm_auth_decrypt(&ctx, length, iv_buf, iv_length, add, add_length, expected_tag, tag_len, ciphertext, decryptedtext) == 0);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, length);

    esp_aes_gcm_free(&ctx);

    heap_caps_free(plaintext);
    heap_caps_free(ciphertext);
    heap_caps_free(decryptedtext);
}
#endif /* SOC_AES_SUPPORT_DMA */

TEST(aes, cbc_aes_256_block_test)
{
    const uint8_t expected_cipher_end[32] = {
        0x3e, 0x68, 0x8a, 0x02, 0xe6, 0xf2, 0x6a, 0x9e,
        0x9b, 0xb2, 0xc0, 0xc4, 0x63, 0x63, 0xd9, 0x25,
        0x51, 0xdc, 0xc2, 0x71, 0x96, 0xb3, 0xe5, 0xcd,
        0xbd, 0x0e, 0xf2, 0xef, 0xa9, 0xab, 0xab, 0x2d,
    };

    test_cbc_aes(0,AES_BUFFER_SIZE, expected_cipher_end);
}

TEST(aes, ctr_aes_256_block_test)
{
    const uint8_t expected_cipher_end[32] = {
        0xed, 0xa4, 0xa4, 0xe0, 0xee, 0x1d, 0x73, 0x96,
        0xd3, 0xde, 0xaa, 0xe0, 0xb7, 0x76, 0x7f, 0xcb,
        0x0f, 0xe8, 0x64, 0xf0, 0xd3, 0xf1, 0xab, 0x14,
        0x5a, 0x89, 0x47, 0xb4, 0x32, 0xed, 0x41, 0x9c,
    };

    test_ctr_aes(0, AES_BUFFER_SIZE, expected_cipher_end);
}

#if SOC_AES_SUPPORT_DMA

TEST(aes, cbc_aes_256_dma_test)
{
    const uint8_t expected_cipher_end[32] = {
        0x3e, 0x68, 0x8a, 0x02, 0xe6, 0xf2, 0x6a, 0x9e,
        0x9b, 0xb2, 0xc0, 0xc4, 0x63, 0x63, 0xd9, 0x25,
        0x51, 0xdc, 0xc2, 0x71, 0x96, 0xb3, 0xe5, 0xcd,
        0xbd, 0x0e, 0xf2, 0xef, 0xa9, 0xab, 0xab, 0x2d,
    };
    test_cbc_aes(1, AES_BUFFER_SIZE, expected_cipher_end);
}

TEST(aes, ctr_aes_256_dma_test)
{
    const uint8_t expected_cipher_end[32] = {
        0xed, 0xa4, 0xa4, 0xe0, 0xee, 0x1d, 0x73, 0x96,
        0xd3, 0xde, 0xaa, 0xe0, 0xb7, 0x76, 0x7f, 0xcb,
        0x0f, 0xe8, 0x64, 0xf0, 0xd3, 0xf1, 0xab, 0x14,
        0x5a, 0x89, 0x47, 0xb4, 0x32, 0xed, 0x41, 0x9c,
    };
    test_ctr_aes(1, AES_BUFFER_SIZE, expected_cipher_end);
}

TEST(aes, ofb_aes_256_dma_test)
{
    const uint8_t expected_cipher_end[] = {
        0x9e, 0x12, 0x10, 0xf0, 0x3f, 0xbf, 0xf8, 0x34,
        0x08, 0x86, 0x7c, 0x02, 0x6b, 0x8a, 0x76, 0xa6,
        0x25, 0x9f, 0x34, 0x61, 0x8b, 0x89, 0x60, 0x16,
        0xe6, 0xa0, 0xa5, 0xb6, 0x5b, 0x0a, 0xeb, 0x1f,
    };
    test_ofb_aes(AES_BUFFER_SIZE, expected_cipher_end);
}

TEST(aes, cfb8_aes_256_dma_test)
{
    const uint8_t expected_cipher_end[] = {
        0x76, 0x95, 0x22, 0x72, 0x3f, 0x44, 0x2d, 0x32,
        0x3e, 0x85, 0xb8, 0xe8, 0xf7, 0x38, 0x04, 0xd6,
        0x4a, 0xc5, 0xdb, 0x2c, 0x46, 0x5f, 0x5b, 0xa2,
        0x24, 0x4a, 0x35, 0xcb, 0xe5, 0x94, 0x71, 0x21,
    };
    test_cfb8_aes(AES_BUFFER_SIZE, expected_cipher_end);
}

TEST(aes, cfb128_aes_256_dma_test)
{
    const uint8_t expected_cipher_end[] = {
        0xd0, 0x9b, 0x2e, 0x25, 0xd5, 0xeb, 0x08, 0xbd,
        0xd8, 0x7e, 0x64, 0xde, 0x35, 0x2b, 0xb1, 0x53,
        0xf8, 0x3a, 0xf7, 0xa8, 0x1e, 0x96, 0xaa, 0xce,
        0xa4, 0xf2, 0x8a, 0x2d, 0x01, 0xd5, 0x62, 0xa0,
    };
    test_cfb128_aes(AES_BUFFER_SIZE, expected_cipher_end);
}

#if CONFIG_CRYPTO_TESTAPP_USE_AES_INTERRUPT

TEST(aes, cbc_aes_256_long_dma_test)
{
    const uint8_t expected_cipher_end[32] = {
        0xd1, 0x32, 0x62, 0x9d, 0x2f, 0x0e, 0x1d, 0x27,
        0x0e, 0x2b, 0x53, 0x0b, 0x81, 0x53, 0x92, 0x69,
        0x8a, 0x9c, 0x25, 0xb1, 0x77, 0x2b, 0xe4, 0x80,
        0x3a, 0xee, 0xdc, 0xbb, 0x80, 0xd6, 0x1a, 0x42,
    };
    test_cbc_aes(1, AES_LONG_BUFFER_SIZE, expected_cipher_end);
}

TEST(aes, ctr_aes_256_long_dma_test)
{
    const uint8_t expected_cipher_end[32] = {
        0x30, 0x8e, 0x3b, 0x27, 0x54, 0x85, 0x58, 0x20,
        0x1a, 0xa6, 0xca, 0x81, 0x12, 0x23, 0x7f, 0x01,
        0xba, 0x27, 0x72, 0x44, 0xa9, 0x00, 0x42, 0x8a,
        0x4e, 0xda, 0x26, 0xf9, 0xd9, 0x0b, 0xb1, 0xa5,
    };
    test_ctr_aes(1, AES_LONG_BUFFER_SIZE, expected_cipher_end);
}

TEST(aes, ofb_aes_256_long_dma_test)
{
    const uint8_t expected_cipher_end[] = {
        0xdc, 0xd1, 0x8a, 0x5c, 0x38, 0xb4, 0xce, 0xdf,
        0x21, 0xa0, 0xa4, 0x0b, 0x87, 0xbb, 0xdf, 0xf5,
        0x42, 0xc6, 0xe2, 0x1f, 0x9f, 0x93, 0x3b, 0xa4,
        0xdd, 0xb0, 0xce, 0xf0, 0x98, 0x47, 0x23, 0x20,
    };
    test_ofb_aes(AES_LONG_BUFFER_SIZE, expected_cipher_end);
}

TEST(aes, cfb8_aes_256_long_dma_test)
{
    const uint8_t expected_cipher_end[] = {
        0x9a, 0x2a, 0xaf, 0xec, 0xd1, 0xf3, 0xd2, 0xe2,
        0xf5, 0x62, 0x16, 0x5c, 0x42, 0x8f, 0xc1, 0xa3,
        0x34, 0x05, 0x9b, 0xa5, 0x44, 0x02, 0xff, 0xf4,
        0x6b, 0xca, 0x3c, 0xac, 0xff, 0x6e, 0xb6, 0x7a,
    };
    test_cfb8_aes(AES_LONG_BUFFER_SIZE, expected_cipher_end);
}

TEST(aes, cfb128_aes_256_long_dma_test)
{
    const uint8_t expected_cipher_end[] = {
        0x6c, 0x63, 0xa9, 0x19, 0x12, 0x89, 0x57, 0xeb,
        0xbe, 0x73, 0x17, 0x62, 0xc6, 0xfc, 0xf0, 0x43,
        0x6d, 0x49, 0x6b, 0xc6, 0x35, 0xf8, 0xc1, 0x48,
        0xe2, 0xb7, 0xb1, 0x6f, 0x26, 0x9f, 0x04, 0x8b,
    };
    test_cfb128_aes(AES_LONG_BUFFER_SIZE, expected_cipher_end);
}

#endif

TEST(aes, gcm_aes_dma_test)
{
    size_t length = 16;
    const uint8_t expected_last_block[16] = {
        0x37, 0x99, 0x4b, 0x16, 0x5f, 0x8d, 0x27, 0xb1,
        0x60, 0x72, 0x9a, 0x81, 0x8d, 0x3c, 0x69, 0x66};

    const uint8_t expected_tag[16] = {
        0x45, 0xc2, 0xa8, 0xfe, 0xff, 0x49, 0x1f, 0x45,
        0x8e, 0x29, 0x74, 0x41, 0xed, 0x9b, 0x54, 0x28};

    test_gcm_aes(length, expected_last_block, expected_tag);
}

#if CONFIG_CRYPTO_TESTAPP_USE_AES_INTERRUPT

TEST(aes, gcm_aes_long_dma_test)
{
    size_t length = 5000;
    const uint8_t expected_last_block[16] = {
        0xee, 0xfd, 0xab, 0x2a, 0x09, 0x44, 0x41, 0x6a,
        0x91, 0xb0, 0x74, 0x24, 0xee, 0x35, 0xb1, 0x39};

    const uint8_t expected_tag[16] = {
        0x22, 0xe1, 0x22, 0x34, 0x0c, 0x91, 0x0b, 0xcf,
        0xa3, 0x42, 0xe0, 0x48, 0xe6, 0xfe, 0x2e, 0x28};

    test_gcm_aes(length, expected_last_block, expected_tag);
}
#endif /* CONFIG_CRYPTO_TESTAPP_USE_AES_INTERRUPT */
#endif /* SOC_AES_SUPPORT_DMA */

TEST_GROUP_RUNNER(aes)
{
    RUN_TEST_CASE(aes, cbc_aes_256_block_test);
    RUN_TEST_CASE(aes, ctr_aes_256_block_test);
#if SOC_AES_SUPPORT_DMA
    RUN_TEST_CASE(aes, cbc_aes_256_dma_test);
    RUN_TEST_CASE(aes, ctr_aes_256_dma_test);
    RUN_TEST_CASE(aes, ofb_aes_256_dma_test);
    RUN_TEST_CASE(aes, cfb8_aes_256_dma_test);
    RUN_TEST_CASE(aes, cfb128_aes_256_dma_test);
#if CONFIG_CRYPTO_TESTAPP_USE_AES_INTERRUPT
    RUN_TEST_CASE(aes, cbc_aes_256_long_dma_test);
    RUN_TEST_CASE(aes, ctr_aes_256_long_dma_test);
    RUN_TEST_CASE(aes, ofb_aes_256_long_dma_test);
    RUN_TEST_CASE(aes, cfb8_aes_256_long_dma_test);
    RUN_TEST_CASE(aes, cfb128_aes_256_long_dma_test);
#endif /* CONFIG_CRYPTO_TESTAPP_USE_AES_INTERRUPT */
    RUN_TEST_CASE(aes, gcm_aes_dma_test);
#if CONFIG_CRYPTO_TESTAPP_USE_AES_INTERRUPT
    RUN_TEST_CASE(aes, gcm_aes_long_dma_test);
#endif /* CONFIG_CRYPTO_TESTAPP_USE_AES_INTERRUPT */
#endif /* SOC_AES_SUPPORT_DMA */
}

#endif // SOC_AES_SUPPORTED
