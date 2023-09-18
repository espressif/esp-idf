/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "soc/soc_caps.h"
#include "esp_heap_caps.h"
#include "unity.h"
#include "test_params.h"
#include "memory_checks.h"
#include "unity_fixture.h"


#define CBC_AES_BUFFER_SIZE 1600
#define CTR_AES_BUFFER_SIZE 1000

#if SOC_AES_SUPPORTED

#include "aes_block.h"

TEST_GROUP(aes);

TEST_SETUP(aes)
{
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(aes)
{
    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}


TEST(aes, cbc_aes_256_test)
{
    uint8_t key_bytes = 256 / 8;
    uint8_t nonce[16];

    const uint8_t expected_cipher_end[] = {
        0x3e, 0x68, 0x8a, 0x02, 0xe6, 0xf2, 0x6a, 0x9e,
        0x9b, 0xb2, 0xc0, 0xc4, 0x63, 0x63, 0xd9, 0x25,
        0x51, 0xdc, 0xc2, 0x71, 0x96, 0xb3, 0xe5, 0xcd,
        0xbd, 0x0e, 0xf2, 0xef, 0xa9, 0xab, 0xab, 0x2d,
    };

    uint8_t *chipertext = heap_caps_calloc(CBC_AES_BUFFER_SIZE, sizeof(uint8_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(chipertext);
    uint8_t *plaintext = heap_caps_calloc(CBC_AES_BUFFER_SIZE, sizeof(uint8_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(plaintext);
    uint8_t *decryptedtext = heap_caps_calloc(CBC_AES_BUFFER_SIZE, sizeof(uint8_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, CBC_AES_BUFFER_SIZE);
    memset(decryptedtext, 0x0, CBC_AES_BUFFER_SIZE);

    // Encrypt
    memcpy(nonce, iv, 16);
    aes_crypt_cbc_block(ESP_AES_ENCRYPT, key_bytes, key_256, CBC_AES_BUFFER_SIZE, nonce, plaintext, chipertext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_end, chipertext + CBC_AES_BUFFER_SIZE - 32, 32);

    // Decrypt
    memcpy(nonce, iv, 16);
    aes_crypt_cbc_block(ESP_AES_DECRYPT, key_bytes, key_256, CBC_AES_BUFFER_SIZE, nonce, chipertext, decryptedtext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, CBC_AES_BUFFER_SIZE);

    // Free dynamically allocated memory
    heap_caps_free(chipertext);
    heap_caps_free(plaintext);
    heap_caps_free(decryptedtext);
}


TEST(aes, ctr_aes_256_test)
{
    uint8_t key_bytes = 256 / 8;
    uint8_t nonce[16];
    uint8_t stream_block[16];
    size_t nc_off = 0;

    const uint8_t expected_cipher_end[] = {
        0xd4, 0xdc, 0x4f, 0x8f, 0xfe, 0x86, 0xee, 0xb5,
        0x14, 0x7f, 0xba, 0x30, 0x25, 0xa6, 0x7f, 0x6c,
        0xb5, 0x73, 0xaf, 0x90, 0xd7, 0xff, 0x36, 0xba,
        0x2b, 0x1d, 0xec, 0xb9, 0x38, 0xfa, 0x0d, 0xeb,
    };

    uint8_t *chipertext = heap_caps_calloc(CTR_AES_BUFFER_SIZE, sizeof(uint8_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(chipertext);
    uint8_t *plaintext = heap_caps_calloc(CTR_AES_BUFFER_SIZE, sizeof(uint8_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(plaintext);
    uint8_t *decryptedtext = heap_caps_calloc(CTR_AES_BUFFER_SIZE, sizeof(uint8_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, CTR_AES_BUFFER_SIZE);
    memset(decryptedtext, 0x0, CTR_AES_BUFFER_SIZE);

    // Encrypt
    memcpy(nonce, iv, 16);
    aes_crypt_ctr_block(key_bytes, key_256, CTR_AES_BUFFER_SIZE, &nc_off, nonce, stream_block, plaintext, chipertext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_end, chipertext + CTR_AES_BUFFER_SIZE - 32, 32);

    // Decrypt
    nc_off = 0;
    memcpy(nonce, iv, 16);
    aes_crypt_ctr_block(key_bytes, key_256, CTR_AES_BUFFER_SIZE, &nc_off, nonce, stream_block, chipertext, decryptedtext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, CTR_AES_BUFFER_SIZE);

    // Free dynamically allocated memory
    heap_caps_free(chipertext);
    heap_caps_free(plaintext);
    heap_caps_free(decryptedtext);
}

#endif // SOC_AES_SUPPORTED

TEST_GROUP_RUNNER(aes)
{
#if SOC_AES_SUPPORTED
    RUN_TEST_CASE(aes, cbc_aes_256_test);
    RUN_TEST_CASE(aes, ctr_aes_256_test);
#endif // SOC_AES_SUPPORTED
}
