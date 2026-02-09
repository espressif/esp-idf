/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
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

#if SOC_AES_SUPPORTED
#include "aes/esp_aes.h"
#include "aes/esp_aes_gcm.h"

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

static void test_cbc_aes(size_t buffer_size, const uint8_t expected_cipher_end[32])
{
    esp_aes_context ctx;
    uint8_t nonce[16];

    esp_aes_init(&ctx);
    TEST_ASSERT_EQUAL(0, esp_aes_setkey(&ctx, key_256, KEY_BITS));

    uint8_t *ciphertext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(ciphertext);
    uint8_t *plaintext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(plaintext);
    uint8_t *decryptedtext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, buffer_size);
    memset(decryptedtext, 0x0, buffer_size);

    // Encrypt
    memcpy(nonce, iv, 16);
    TEST_ASSERT_EQUAL(0, esp_aes_crypt_cbc(&ctx, ESP_AES_ENCRYPT, buffer_size, nonce, plaintext, ciphertext));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_end, ciphertext + buffer_size - 32, 32);

    // Decrypt
    memcpy(nonce, iv, 16);
    TEST_ASSERT_EQUAL(0, esp_aes_crypt_cbc(&ctx, ESP_AES_DECRYPT, buffer_size, nonce, ciphertext, decryptedtext));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, buffer_size);

    esp_aes_free(&ctx);

    // Free dynamically allocated memory
    heap_caps_free(ciphertext);
    heap_caps_free(plaintext);
    heap_caps_free(decryptedtext);
}

static void test_ctr_aes(size_t buffer_size, const uint8_t expected_cipher_end[32])
{
    esp_aes_context ctx;
    uint8_t nonce[16];
    uint8_t stream_block[16];
    size_t nc_off = 0;

    esp_aes_init(&ctx);
    TEST_ASSERT_EQUAL(0, esp_aes_setkey(&ctx, key_256, KEY_BITS));

    uint8_t *ciphertext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(ciphertext);
    uint8_t *plaintext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(plaintext);
    uint8_t *decryptedtext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, buffer_size);
    memset(decryptedtext, 0x0, buffer_size);

    // Encrypt
    memcpy(nonce, iv, 16);
    TEST_ASSERT_EQUAL(0, esp_aes_crypt_ctr(&ctx, buffer_size, &nc_off, nonce, stream_block, plaintext, ciphertext));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_end, ciphertext + buffer_size - 32, 32);

    // Decrypt
    memcpy(nonce, iv, 16);
    nc_off = 0;
    TEST_ASSERT_EQUAL(0, esp_aes_crypt_ctr(&ctx, buffer_size, &nc_off, nonce, stream_block, ciphertext, decryptedtext));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, buffer_size);

    esp_aes_free(&ctx);

    // Free dynamically allocated memory
    heap_caps_free(ciphertext);
    heap_caps_free(plaintext);
    heap_caps_free(decryptedtext);
}

#if SOC_AES_SUPPORT_DMA
static void test_ofb_aes(size_t buffer_size, const uint8_t expected_cipher_end[32])
{
    esp_aes_context ctx;
    uint8_t nonce[16];
    size_t nc_off = 0;

    esp_aes_init(&ctx);
    TEST_ASSERT_EQUAL(0, esp_aes_setkey(&ctx, key_256, KEY_BITS));

    uint8_t *ciphertext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(ciphertext);
    uint8_t *plaintext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(plaintext);
    uint8_t *decryptedtext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, buffer_size);
    memset(decryptedtext, 0x0, buffer_size);

    // Encrypt
    memcpy(nonce, iv, 16);
    TEST_ASSERT_EQUAL(0, esp_aes_crypt_ofb(&ctx, buffer_size, &nc_off, nonce, plaintext, ciphertext));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_end, ciphertext + buffer_size - 32, 32);

    // Decrypt
    memcpy(nonce, iv, 16);
    nc_off = 0;
    TEST_ASSERT_EQUAL(0, esp_aes_crypt_ofb(&ctx, buffer_size, &nc_off, nonce, ciphertext, decryptedtext));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, buffer_size);

    esp_aes_free(&ctx);

    // Free dynamically allocated memory
    heap_caps_free(ciphertext);
    heap_caps_free(plaintext);
    heap_caps_free(decryptedtext);
}

static void test_cfb8_aes(size_t buffer_size, const uint8_t expected_cipher_end[32])
{
    esp_aes_context ctx;
    uint8_t nonce[16];

    esp_aes_init(&ctx);
    TEST_ASSERT_EQUAL(0, esp_aes_setkey(&ctx, key_256, KEY_BITS));

    uint8_t *ciphertext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(ciphertext);
    uint8_t *plaintext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(plaintext);
    uint8_t *decryptedtext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, buffer_size);
    memset(decryptedtext, 0x0, buffer_size);

    // Encrypt
    memcpy(nonce, iv, 16);
    TEST_ASSERT_EQUAL(0, esp_aes_crypt_cfb8(&ctx, ESP_AES_ENCRYPT, buffer_size, nonce, plaintext, ciphertext));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_end, ciphertext + buffer_size - 32, 32);

    // Decrypt
    memcpy(nonce, iv, 16);
    TEST_ASSERT_EQUAL(0, esp_aes_crypt_cfb8(&ctx, ESP_AES_DECRYPT, buffer_size, nonce, ciphertext, decryptedtext));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, buffer_size);

    esp_aes_free(&ctx);

    // Free dynamically allocated memory
    heap_caps_free(ciphertext);
    heap_caps_free(plaintext);
    heap_caps_free(decryptedtext);
}

static void test_cfb128_aes(size_t buffer_size, const uint8_t expected_cipher_end[32])
{
    esp_aes_context ctx;
    uint8_t nonce[16];
    size_t nc_off = 0;

    esp_aes_init(&ctx);
    TEST_ASSERT_EQUAL(0, esp_aes_setkey(&ctx, key_256, KEY_BITS));

    uint8_t *ciphertext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(ciphertext);
    uint8_t *plaintext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(plaintext);
    uint8_t *decryptedtext = heap_caps_calloc(buffer_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, buffer_size);
    memset(decryptedtext, 0x0, buffer_size);

    // Encrypt
    memcpy(nonce, iv, 16);
    TEST_ASSERT_EQUAL(0, esp_aes_crypt_cfb128(&ctx, ESP_AES_ENCRYPT, buffer_size, &nc_off, nonce, plaintext, ciphertext));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_end, ciphertext + buffer_size - 32, 32);

    // Decrypt
    nc_off = 0;
    memcpy(nonce, iv, 16);
    TEST_ASSERT_EQUAL(0, esp_aes_crypt_cfb128(&ctx, ESP_AES_DECRYPT, buffer_size, &nc_off, nonce, ciphertext, decryptedtext));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, buffer_size);

    esp_aes_free(&ctx);

    // Free dynamically allocated memory
    heap_caps_free(ciphertext);
    heap_caps_free(plaintext);
    heap_caps_free(decryptedtext);
}

#if CONFIG_SOC_AES_SUPPORT_GCM
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
    TEST_ASSERT_EQUAL(0, esp_aes_gcm_setkey(&ctx, CIPHER_ID_AES, key, 8 * sizeof(key)));

    /* Encrypt and authenticate */
    TEST_ASSERT_EQUAL(0, esp_aes_gcm_crypt_and_tag(&ctx, ESP_AES_ENCRYPT, length, iv_buf, iv_length, add, add_length, plaintext, ciphertext, tag_len, tag_buf_encrypt));
    size_t offset = length > 16 ? length - 16 : 0;
    /* Sanity check: make sure the last ciphertext block matches what we expect to see. */
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_last_block, ciphertext + offset, MIN(16, length));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_tag, tag_buf_encrypt, tag_len);

    /* Decrypt and authenticate */
    TEST_ASSERT_EQUAL(0, esp_aes_gcm_auth_decrypt(&ctx, length, iv_buf, iv_length, add, add_length, expected_tag, tag_len, ciphertext, decryptedtext));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, length);

    esp_aes_gcm_free(&ctx);

    heap_caps_free(plaintext);
    heap_caps_free(ciphertext);
    heap_caps_free(decryptedtext);
}
#endif /* SOC_GCM_SUPPORTED */
#endif /* SOC_AES_SUPPORT_DMA */

TEST(aes, cbc_aes_256_block_test)
{
    test_cbc_aes(AES_BUFFER_SIZE_BLOCK_MODE, expected_cipher_end_block_cbc);
}

TEST(aes, ctr_aes_256_block_test)
{
    test_ctr_aes(AES_BUFFER_SIZE_BLOCK_MODE, expected_cipher_end_block_ctr);
}

#if SOC_AES_SUPPORT_DMA
TEST(aes, cbc_aes_256_dma_test)
{
    test_cbc_aes(AES_BUFFER_SIZE_DMA_MODE, expected_cipher_end_dma_cbc);
}

TEST(aes, ctr_aes_256_dma_test)
{
    test_ctr_aes(AES_BUFFER_SIZE_DMA_MODE, expected_cipher_end_dma_ctr);
}

TEST(aes, ofb_aes_256_dma_test)
{
    test_ofb_aes(AES_BUFFER_SIZE_DMA_MODE, expected_cipher_end_dma_ofb);
}

TEST(aes, cfb8_aes_256_dma_test)
{
    test_cfb8_aes(AES_BUFFER_SIZE_DMA_MODE, expected_cipher_end_dma_cfb8);
}

TEST(aes, cfb128_aes_256_dma_test)
{
    test_cfb128_aes(AES_BUFFER_SIZE_DMA_MODE, expected_cipher_end_dma_cfb128);
}

#if CONFIG_CRYPTO_TESTAPP_USE_AES_INTERRUPT

TEST(aes, cbc_aes_256_long_dma_test)
{
    test_cbc_aes(AES_BUFFER_SIZE_DMA_MODE_WITH_INTERRUPT, expected_cipher_end_dma_cbc_with_interrupt);
}

TEST(aes, ctr_aes_256_long_dma_test)
{
    test_ctr_aes(AES_BUFFER_SIZE_DMA_MODE_WITH_INTERRUPT, expected_cipher_end_dma_ctr_with_interrupt);
}

TEST(aes, ofb_aes_256_long_dma_test)
{
    test_ofb_aes(AES_BUFFER_SIZE_DMA_MODE_WITH_INTERRUPT, expected_cipher_end_dma_ofb_with_interrupt);
}

TEST(aes, cfb8_aes_256_long_dma_test)
{
    test_cfb8_aes(AES_BUFFER_SIZE_DMA_MODE_WITH_INTERRUPT, expected_cipher_end_dma_cfb8_with_interrupt);
}

TEST(aes, cfb128_aes_256_long_dma_test)
{
    test_cfb128_aes(AES_BUFFER_SIZE_DMA_MODE_WITH_INTERRUPT, expected_cipher_end_dma_cfb128_with_interrupt);
}

#endif

#if CONFIG_SOC_AES_SUPPORT_GCM
TEST(aes, gcm_aes_dma_test)
{
    size_t length = 16;
    const uint8_t expected_last_block[16] = {
        0x37, 0x99, 0x4b, 0x16, 0x5f, 0x8d, 0x27, 0xb1,
        0x60, 0x72, 0x9a, 0x81, 0x8d, 0x3c, 0x69, 0x66
    };

    const uint8_t expected_tag[16] = {
        0x45, 0xc2, 0xa8, 0xfe, 0xff, 0x49, 0x1f, 0x45,
        0x8e, 0x29, 0x74, 0x41, 0xed, 0x9b, 0x54, 0x28
    };

    test_gcm_aes(length, expected_last_block, expected_tag);
}

#if CONFIG_CRYPTO_TESTAPP_USE_AES_INTERRUPT

TEST(aes, gcm_aes_long_dma_test)
{
    size_t length = 5000;
    const uint8_t expected_last_block[16] = {
        0xee, 0xfd, 0xab, 0x2a, 0x09, 0x44, 0x41, 0x6a,
        0x91, 0xb0, 0x74, 0x24, 0xee, 0x35, 0xb1, 0x39
    };

    const uint8_t expected_tag[16] = {
        0x22, 0xe1, 0x22, 0x34, 0x0c, 0x91, 0x0b, 0xcf,
        0xa3, 0x42, 0xe0, 0x48, 0xe6, 0xfe, 0x2e, 0x28
    };

    test_gcm_aes(length, expected_last_block, expected_tag);
}
#endif /* CONFIG_CRYPTO_TESTAPP_USE_AES_INTERRUPT */
#endif /* SOC_GCM_SUPPORTED */
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
#if CONFIG_SOC_AES_SUPPORT_GCM
    RUN_TEST_CASE(aes, gcm_aes_dma_test);
#if CONFIG_CRYPTO_TESTAPP_USE_AES_INTERRUPT
    RUN_TEST_CASE(aes, gcm_aes_long_dma_test);
#endif /* CONFIG_CRYPTO_TESTAPP_USE_AES_INTERRUPT */
#endif /* CONFIG_SOC_AES_SUPPORT_GCM */
#endif /* SOC_AES_SUPPORT_DMA */
}

#endif // SOC_AES_SUPPORTED
