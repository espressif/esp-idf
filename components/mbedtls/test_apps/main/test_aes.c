/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* mbedTLS AES test
*/
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <esp_system.h>
#include "mbedtls/aes.h"
#include "mbedtls/gcm.h"
#include "unity.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "test_utils.h"
#include "test_aes_params.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_memory_utils.h"
#include "soc/lldesc.h"


static const uint8_t key_256[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
};

static const uint8_t iv[] = {
    0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09,
    0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,
};

TEST_CASE("mbedtls CBC AES-256 test", "[aes]")
{
    const unsigned SZ = 1600;
    mbedtls_aes_context ctx;
    uint8_t nonce[16];

    memcpy(nonce, iv, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key_256, 256);

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    // Encrypt
    mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, SZ, nonce, plaintext, ciphertext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_cbc_end, ciphertext + SZ - 32, 32);

    // Decrypt
    memcpy(nonce, iv, 16);
    mbedtls_aes_setkey_dec(&ctx, key_256, 256);
    mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, SZ, nonce, ciphertext, decryptedtext);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    mbedtls_aes_free(&ctx);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls CBC AES-256 DMA buffer align test", "[aes]")
{
#define ALIGN_DOWN(val, align) ((val) & ~((align) - 1))
    // Size is taken considering the maximum DMA buffer size
    const unsigned SZ = ALIGN_DOWN((2*LLDESC_MAX_NUM_PER_DESC), 16);
    mbedtls_aes_context ctx;
    uint8_t nonce[16];

    memcpy(nonce, iv, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key_256, 256);

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    // Encrypt
    mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, SZ, nonce, plaintext, ciphertext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_cbc_align_end, ciphertext + SZ - 32, 32);

    // Decrypt
    memcpy(nonce, iv, 16);
    mbedtls_aes_setkey_dec(&ctx, key_256, 256);
    mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, SZ, nonce, ciphertext, decryptedtext);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    mbedtls_aes_free(&ctx);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls CTR AES-256 test", "[aes]")
{
    const unsigned SZ = 1000;
    mbedtls_aes_context ctx;
    uint8_t nonce[16];
    uint8_t stream_block[16];
    size_t nc_off = 0;

    memcpy(nonce, iv, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key_256, 256);

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    // Encrypt
    mbedtls_aes_crypt_ctr(&ctx, SZ, &nc_off, nonce, stream_block, plaintext, ciphertext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_ctr_end, ciphertext + SZ - 32, 32);

    // Decrypt
    nc_off = 0;
    memcpy(nonce, iv, 16);
    mbedtls_aes_crypt_ctr(&ctx, SZ, &nc_off, nonce, stream_block, ciphertext, decryptedtext);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    mbedtls_aes_free(&ctx);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls OFB AES-256 test", "[aes]")
{
    const unsigned SZ = 1000;
    mbedtls_aes_context ctx;
    uint8_t nonce[16];
    size_t nc_off = 0;

    memcpy(nonce, iv, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key_256, 256);

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    // Encrypt
    mbedtls_aes_crypt_ofb(&ctx, SZ, &nc_off, nonce, plaintext, ciphertext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_ofb_end, ciphertext + SZ - 32, 32);

    // Decrypt
    nc_off = 0;
    memcpy(nonce, iv, 16);
    mbedtls_aes_crypt_ofb(&ctx, SZ, &nc_off, nonce, ciphertext, decryptedtext);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    mbedtls_aes_free(&ctx);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls CFB-8 AES-256 test", "[aes]")
{
    const unsigned SZ = 1000;
    mbedtls_aes_context ctx;
    uint8_t nonce[16];

    memcpy(nonce, iv, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key_256, 256);

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    // Encrypt
    mbedtls_aes_crypt_cfb8(&ctx, MBEDTLS_AES_ENCRYPT, SZ, nonce, plaintext, ciphertext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_cfb8_end, ciphertext + SZ - 32, 32);

    // Decrypt
    memcpy(nonce, iv, 16);
    mbedtls_aes_crypt_cfb8(&ctx, MBEDTLS_AES_DECRYPT, SZ, nonce, ciphertext, decryptedtext);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    mbedtls_aes_free(&ctx);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls CFB-128 AES-256 test", "[aes]")
{
    const unsigned SZ = 1000;
    mbedtls_aes_context ctx;
    uint8_t nonce[16];
    size_t nc_off = 0;

    memcpy(nonce, iv, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key_256, 256);

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    // Encrypt
    mbedtls_aes_crypt_cfb128(&ctx, MBEDTLS_AES_ENCRYPT,  SZ, &nc_off, nonce, plaintext, ciphertext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_cfb128_end, ciphertext + SZ - 32, 32);

    // Decrypt
    nc_off = 0;
    memcpy(nonce, iv, 16);
    mbedtls_aes_crypt_cfb128(&ctx, MBEDTLS_AES_DECRYPT, SZ, &nc_off, nonce, ciphertext, decryptedtext);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    mbedtls_aes_free(&ctx);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

static void aes_ctr_stream_test(void)
{
    const unsigned SZ = 100;
    mbedtls_aes_context ctx;
    uint8_t nonce[16];
    uint8_t key[16];
    uint8_t stream_block[16];

    memset(nonce, 0xEE, 16);
    memset(key, 0x44, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key, 128);
    memset(plaintext, 0xAA, SZ);

    /* Test that all the end results are the same
        no matter how many bytes we encrypt each call
        */
    for (int bytes_to_process = 1; bytes_to_process < SZ; bytes_to_process++) {
        ESP_LOGD("test", "bytes_to_process %d", bytes_to_process);
        memset(nonce, 0xEE, 16);
        memset(ciphertext, 0x0, SZ);
        memset(decryptedtext, 0x0, SZ);

        size_t offset = 0;
        // Encrypt
        for (int idx = 0; idx < SZ; idx = idx + bytes_to_process) {
            // Limit length of last call to avoid exceeding buffer size
            size_t length = (idx + bytes_to_process > SZ) ? (SZ - idx) : bytes_to_process;

            mbedtls_aes_crypt_ctr(&ctx, length, &offset, nonce,
                                  stream_block, plaintext + idx, ciphertext + idx );
        }
        ESP_LOG_BUFFER_HEXDUMP("expected", expected_cipher_ctr_stream, SZ, ESP_LOG_DEBUG);
        ESP_LOG_BUFFER_HEXDUMP("actual  ", ciphertext, SZ, ESP_LOG_DEBUG);

        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_ctr_stream, ciphertext, SZ);

        // Decrypt
        memset(nonce, 0xEE, 16);
        memset(decryptedtext, 0x22, SZ);
        offset = 0;
        for (int idx = 0; idx < SZ; idx = idx + bytes_to_process) {
            // Limit length of last call to avoid exceeding buffer size
            size_t length = (idx + bytes_to_process > SZ) ? (SZ - idx) : bytes_to_process;
            mbedtls_aes_crypt_ctr(&ctx, length, &offset, nonce,
                                  stream_block, ciphertext + idx, decryptedtext + idx );
        }
        ESP_LOG_BUFFER_HEXDUMP("decrypted", decryptedtext, SZ, ESP_LOG_DEBUG);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);
    }

    mbedtls_aes_free(&ctx);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls CTR stream test", "[aes]")
{
    aes_ctr_stream_test();
}


TEST_CASE("mbedtls OFB stream test", "[aes]")
{
    const unsigned SZ = 100;
    mbedtls_aes_context ctx;
    uint8_t iv[16];
    uint8_t key[16];

    memset(key, 0x44, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key, 128);
    memset(plaintext, 0xAA, SZ);

    /* Test that all the end results are the same
        no matter how many bytes we encrypt each call
        */

    for (int bytes_to_process = 1; bytes_to_process < SZ; bytes_to_process++) {
        ESP_LOGD("test", "bytes_to_process %d", bytes_to_process);
        // Encrypt
        memset(iv, 0xEE, 16);
        size_t offset = 0;
        for (int idx = 0; idx < SZ; idx = idx + bytes_to_process) {
            // Limit length of last call to avoid exceeding buffer size
            size_t length = ( (idx + bytes_to_process) > SZ) ? (SZ - idx) : bytes_to_process;
            mbedtls_aes_crypt_ofb(&ctx, length, &offset, iv, plaintext + idx, ciphertext + idx);

        }
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_ofb_stream, ciphertext, SZ);

        // Decrypt
        memset(iv, 0xEE, 16);
        memset(decryptedtext, 0x22, SZ);
        offset = 0;
        for (int idx = 0; idx < SZ; idx = idx + bytes_to_process) {
            // Limit length of last call to avoid exceeding buffer size
            size_t length = (idx + bytes_to_process > SZ) ? (SZ - idx) : bytes_to_process;
            mbedtls_aes_crypt_ofb(&ctx, length, &offset, iv, ciphertext + idx, decryptedtext + idx);
        }
        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);
    }

    mbedtls_aes_free(&ctx);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls CFB8 stream test", "[aes]")
{
    const unsigned SZ = 32;
    mbedtls_aes_context ctx;
    uint8_t iv[16];
    uint8_t key[16];

    memset(key, 0x44, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key, 128);
    memset(plaintext, 0xAA, SZ);

    /* Test that all the end results are the same
        no matter how many bytes we encrypt each call
        */

    for (int bytes_to_process = 1; bytes_to_process < SZ; bytes_to_process++) {
        memset(iv, 0xEE, 16);
        for (int idx = 0; idx < SZ; idx = idx + bytes_to_process) {
            // Limit length of last call to avoid exceeding buffer size
            size_t length = ( (idx + bytes_to_process) > SZ) ? (SZ - idx) : bytes_to_process;
            mbedtls_aes_crypt_cfb8(&ctx, MBEDTLS_AES_ENCRYPT, length, iv, plaintext + idx, ciphertext + idx);

        }
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_cfb8_stream, ciphertext, SZ);

        memset(iv, 0xEE, 16);
        for (int idx = 0; idx < SZ; idx = idx + bytes_to_process) {
            // Limit length of last call to avoid exceeding buffer size
            size_t length = ( (idx + bytes_to_process) > SZ) ? (SZ - idx) : bytes_to_process;
            mbedtls_aes_crypt_cfb8(&ctx, MBEDTLS_AES_DECRYPT, length, iv, ciphertext + idx, decryptedtext + idx);

        }
        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);
    }

    mbedtls_aes_free(&ctx);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls CFB128 stream test", "[aes]")
{
    const unsigned SZ = 32;
    mbedtls_aes_context ctx;
    uint8_t iv[16];
    uint8_t key[16];

    memset(key, 0x44, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key, 128);
    memset(plaintext, 0xAA, SZ);

    /* Test that all the end results are the same
        no matter how many bytes we encrypt each call
    */

    //for (int bytes_to_process = 1; bytes_to_process < SZ; bytes_to_process++) {
    int bytes_to_process = 17;
    size_t offset = 0;
    memset(iv, 0xEE, 16);
    for (int idx = 0; idx < SZ; idx = idx + bytes_to_process) {
        // Limit length of last call to avoid exceeding buffer size
        size_t length = ( (idx + bytes_to_process) > SZ) ? (SZ - idx) : bytes_to_process;
        mbedtls_aes_crypt_cfb128(&ctx, MBEDTLS_AES_ENCRYPT, length, &offset, iv, plaintext + idx, ciphertext + idx);

    }
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_cfb128_stream, ciphertext, SZ);

    offset = 0;
    memset(iv, 0xEE, 16);
    for (int idx = 0; idx < SZ; idx = idx + bytes_to_process) {
        // Limit length of last call to avoid exceeding buffer size
        size_t length = ( (idx + bytes_to_process) > SZ) ? (SZ - idx) : bytes_to_process;
        mbedtls_aes_crypt_cfb128(&ctx, MBEDTLS_AES_DECRYPT, length, &offset, iv, ciphertext + idx, decryptedtext + idx);

    }
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    mbedtls_aes_free(&ctx);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

/* Test the case where the input and output buffers point to the same location */
TEST_CASE("mbedtls CTR, input buf = output buf", "[aes]")
{
    const unsigned SZ = 1000;
    mbedtls_aes_context ctx;
    uint8_t nonce[16];
    uint8_t stream_block[16];
    size_t nc_off = 0;

    memcpy(nonce, iv, 16);

    // allocate internal memory
    uint8_t *buf = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);

    TEST_ASSERT_NOT_NULL(buf);

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key_256, 256);

    memset(buf, 0x3A, SZ);

    // Encrypt
    mbedtls_aes_crypt_ctr(&ctx, SZ, &nc_off, nonce, stream_block, buf, buf);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_ctr_inplace_end, buf + SZ - 32, 32);

    // Decrypt
    nc_off = 0;
    memcpy(nonce, iv, 16);
    mbedtls_aes_crypt_ctr(&ctx, SZ, &nc_off, nonce, stream_block, buf, buf);

    for (int i = 0; i < SZ; i++) {
        TEST_ASSERT_EQUAL_HEX8(0x3A, buf[i]);
    }

    mbedtls_aes_free(&ctx);
    free(buf);
}

TEST_CASE("mbedtls OFB, chained DMA descriptors", "[aes]")
{
    // Max bytes in a single DMA descriptor is 4095
    const unsigned SZ = 6000;
    mbedtls_aes_context ctx;
    uint8_t nonce[16];
    size_t nc_off = 0;

    memcpy(nonce, iv, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key_256, 256);

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    // Encrypt
    mbedtls_aes_crypt_ofb(&ctx, SZ, &nc_off, nonce, plaintext, ciphertext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_ofb_chained_end, ciphertext + SZ - 32, 32);


    // Decrypt
    nc_off = 0;
    memcpy(nonce, iv, 16);
    mbedtls_aes_crypt_ofb(&ctx, SZ, &nc_off, nonce, ciphertext, decryptedtext);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    mbedtls_aes_free(&ctx);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

void aes_ctr_alignment_test(uint32_t input_buf_caps, uint32_t output_buf_caps)
{
    mbedtls_aes_context ctx;
    uint8_t nonce[16];
    uint8_t key[16];
    uint8_t stream_block[16];
    size_t SZ = TEST_AES_CTR_DATA_LEN;
    size_t ALIGNMENT_SIZE_BYTES = 64;
    memset(nonce, 0x2F, 16);
    memset(key, 0x1E, 16);

    // allocate memory according the requested caps
    uint8_t *ciphertext = heap_caps_malloc(SZ + ALIGNMENT_SIZE_BYTES, output_buf_caps);
    uint8_t *plaintext = heap_caps_malloc(SZ + ALIGNMENT_SIZE_BYTES, input_buf_caps);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key, 128);
    memset(plaintext, 0x26, SZ + ALIGNMENT_SIZE_BYTES);

    size_t offset;

    /* Shift buffers and test for all different misalignments */
    for (int i = 0; i < ALIGNMENT_SIZE_BYTES; i++ ) {
        // Encrypt with input buffer in external ram
        offset = 0;
        memset(nonce, 0x2F, 16);
        mbedtls_aes_crypt_ctr(&ctx, SZ, &offset, nonce, stream_block, plaintext + i, ciphertext + i);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_ctr, ciphertext + i, SZ);

        // Decrypt
        offset = 0;
        memset(nonce, 0x2F, 16);
        // Decrypt with input buffer in instruction memory, the crypto DMA can't access this
        mbedtls_aes_crypt_ctr(&ctx, SZ, &offset, nonce, stream_block, ciphertext + i, decryptedtext);

        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    }

    mbedtls_aes_free(&ctx);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls AES internal mem alignment tests", "[aes]")
{
    uint32_t internal_dma_caps = MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL;
    aes_ctr_alignment_test(internal_dma_caps, internal_dma_caps);
}


#ifdef CONFIG_SPIRAM_USE_MALLOC

void aes_psram_one_buf_ctr_test(void)
{
    mbedtls_aes_context ctx;
    uint8_t nonce[16];
    uint8_t key[16];
    uint8_t stream_block[16];
    size_t SZ = TEST_AES_CTR_DATA_LEN;
    size_t ALIGNMENT_SIZE_BYTES = 32;
    memset(nonce, 0x2F, 16);
    memset(key, 0x1E, 16);

    // allocate external memory
    uint8_t *buf = heap_caps_malloc(SZ + ALIGNMENT_SIZE_BYTES, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);

    TEST_ASSERT_NOT_NULL(buf);

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key, 128);
    memset(buf, 0x26, SZ + ALIGNMENT_SIZE_BYTES);

    size_t offset;

    /* Shift buffers and test for all different misalignments */
    for (int i = 0; i < ALIGNMENT_SIZE_BYTES; i++ ) {
        // Encrypt with input buffer in external ram
        offset = 0;
        memset(buf, 0x26, SZ + ALIGNMENT_SIZE_BYTES);
        memset(nonce, 0x2F, 16);
        mbedtls_aes_crypt_ctr(&ctx, SZ, &offset, nonce, stream_block, buf + i, buf + i);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_ctr, buf + i, SZ);

        // Decrypt
        offset = 0;
        memset(nonce, 0x2F, 16);
        // Decrypt with input buffer in instruction memory, the crypto DMA can't access this
        mbedtls_aes_crypt_ctr(&ctx, SZ, &offset, nonce, stream_block, buf + i, buf);

        TEST_ASSERT_EACH_EQUAL_HEX8(0x26, buf + i, SZ - i);

    }
    mbedtls_aes_free(&ctx);
    free(buf);
}

void aes_ext_flash_ctr_test(uint32_t output_buf_caps)
{
    mbedtls_aes_context ctx;
    uint8_t nonce[16];
    uint8_t key[16];
    uint8_t stream_block[16];
    size_t SZ = sizeof(long_input);
    memset(nonce, 0x2F, 16);
    memset(key, 0x1E, 16);

    uint8_t *ciphertext = heap_caps_malloc(SZ, output_buf_caps);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key, 128);

    size_t offset;

    // Encrypt with input buffer in external flash
    offset = 0;
    memset(nonce, 0x2F, 16);
    mbedtls_aes_crypt_ctr(&ctx, SZ, &offset, nonce, stream_block, long_input, ciphertext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_long_input_end, ciphertext + SZ - 32, 32);

    // Decrypt
    offset = 0;
    memset(nonce, 0x2F, 16);
    // Decrypt with input buffer in external flash, the crypto DMA can't access this
    mbedtls_aes_crypt_ctr(&ctx, SZ, &offset, nonce, stream_block, ciphertext, decryptedtext);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(long_input, decryptedtext, SZ);

    mbedtls_aes_free(&ctx);
    free(ciphertext);
    free(decryptedtext);
}

/* Tests how crypto DMA handles data in external memory */
TEST_CASE("mbedtls AES PSRAM tests", "[aes]")
{
    aes_ctr_alignment_test(MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    aes_ctr_alignment_test(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    aes_ctr_alignment_test(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    aes_psram_one_buf_ctr_test();
}

/* Tests how crypto DMA handles data from external flash */
TEST_CASE("mbedtls AES external flash tests", "[aes]")
{
    aes_ext_flash_ctr_test(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    aes_ext_flash_ctr_test(MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
}
#endif // CONFIG_SPIRAM_USE_MALLOC


static SemaphoreHandle_t done_sem;

static void __attribute__((unused)) aes_ctr_stream_test_task(void *pv)
{
    aes_ctr_stream_test();
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

#if CONFIG_ESP_SYSTEM_RTC_FAST_MEM_AS_HEAP_DEPCHECK && !CONFIG_IDF_TARGET_ESP32H2
// Not enough rtc memory for test on H2

TEST_CASE("mbedtls AES stack in RTC RAM", "[mbedtls]")
{
    done_sem = xSemaphoreCreateBinary();
    static StaticTask_t rtc_task;
    size_t STACK_SIZE = 3072;
    uint8_t *rtc_stack = heap_caps_calloc(STACK_SIZE, 1, MALLOC_CAP_RTCRAM);
    TEST_ASSERT(esp_ptr_in_rtc_dram_fast(rtc_stack));

    TEST_ASSERT_NOT_NULL(xTaskCreateStatic(aes_ctr_stream_test_task, "aes_ctr_task", STACK_SIZE, NULL,
                                            3, rtc_stack, &rtc_task));
    TEST_ASSERT_TRUE(xSemaphoreTake(done_sem, 10000 / portTICK_PERIOD_MS));

    /* Give task time to cleanup before freeing stack */
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    free(rtc_stack);

    vSemaphoreDelete(done_sem);
}

#endif //CONFIG_ESP_SYSTEM_RTC_FAST_MEM_AS_HEAP_DEPCHECK && !CONFIG_IDF_TARGET_ESP32H2

#if CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM && CONFIG_SPIRAM_USE_MALLOC

TEST_CASE("mbedtls AES stack in PSRAM", "[mbedtls]")
{
    done_sem = xSemaphoreCreateBinary();
    static StaticTask_t psram_task;
    size_t STACK_SIZE = 3072;
    uint8_t *psram_stack = heap_caps_calloc(STACK_SIZE, 1, MALLOC_CAP_SPIRAM);

    TEST_ASSERT(esp_ptr_external_ram(psram_stack));

    TEST_ASSERT_NOT_NULL(xTaskCreateStatic(aes_ctr_stream_test_task, "aes_ctr_task", STACK_SIZE, NULL,
                                            3, psram_stack, &psram_task));
    TEST_ASSERT_TRUE(xSemaphoreTake(done_sem, 10000 / portTICK_PERIOD_MS));

    /* Give task time to cleanup before freeing stack */
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    free(psram_stack);

    vSemaphoreDelete(done_sem);
}

#endif //CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM && CONFIG_SPIRAM_USE_MALLOC
