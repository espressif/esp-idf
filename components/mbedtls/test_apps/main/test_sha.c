/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "esp_types.h"
#include "esp_log.h"
#include "ccomp_timer.h"
#include "esp_heap_caps.h"
#include "idf_performance.h"
#include "esp_private/esp_clk.h"
#include "spi_flash_mmap.h"

#include "soc/soc_caps.h"
#define MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS
#include "unity.h"
#include "test_utils.h"

#include "sha/sha_parallel_engine.h"
#include "psa/crypto.h"
#include "mbedtls/md.h"
#define TAG "sha_test"

// New test for PSA SHA-512 implementation
TEST_CASE("Test PSA SHA-512 with known test vectors", "[hw_crypto][psa]")
{
    ESP_LOGI(TAG, "Testing PSA SHA-512 implementation with known test vectors");

    // Test Vector 1: SHA-512("abc")
    // Expected: ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f
    const unsigned char test1_input[] = "abc";
    const size_t test1_input_len = 3;
    const unsigned char test1_expected[64] = {
        0xdd, 0xaf, 0x35, 0xa1, 0x93, 0x61, 0x7a, 0xba, 0xcc, 0x41, 0x73, 0x49, 0xae, 0x20, 0x41, 0x31,
        0x12, 0xe6, 0xfa, 0x4e, 0x89, 0xa9, 0x7e, 0xa2, 0x0a, 0x9e, 0xee, 0xe6, 0x4b, 0x55, 0xd3, 0x9a,
        0x21, 0x92, 0x99, 0x2a, 0x27, 0x4f, 0xc1, 0xa8, 0x36, 0xba, 0x3c, 0x23, 0xa3, 0xfe, 0xeb, 0xbd,
        0x45, 0x4d, 0x44, 0x23, 0x64, 0x3c, 0xe8, 0x0e, 0x2a, 0x9a, 0xc9, 0x4f, 0xa5, 0x4c, 0xa4, 0x9f
    };

    // Test Vector 2: SHA-512("")
    // Expected: cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e
    const unsigned char test2_input[] = "";
    const size_t test2_input_len = 0;
    const unsigned char test2_expected[64] = {
        0xcf, 0x83, 0xe1, 0x35, 0x7e, 0xef, 0xb8, 0xbd, 0xf1, 0x54, 0x28, 0x50, 0xd6, 0x6d, 0x80, 0x07,
        0xd6, 0x20, 0xe4, 0x05, 0x0b, 0x57, 0x15, 0xdc, 0x83, 0xf4, 0xa9, 0x21, 0xd3, 0x6c, 0xe9, 0xce,
        0x47, 0xd0, 0xd1, 0x3c, 0x5d, 0x85, 0xf2, 0xb0, 0xff, 0x83, 0x18, 0xd2, 0x87, 0x7e, 0xec, 0x2f,
        0x63, 0xb9, 0x31, 0xbd, 0x47, 0x41, 0x7a, 0x81, 0xa5, 0x38, 0x32, 0x7a, 0xf9, 0x27, 0xda, 0x3e
    };

    unsigned char psa_output[64];
    unsigned char mbedtls_output[64];
    size_t psa_output_len;
    psa_status_t psa_status;
    int mbedtls_ret;

    ESP_LOGI(TAG, "=== Test 1: SHA-512(\"abc\") ===");

    // Test with PSA
    ESP_LOGI(TAG, "Testing PSA psa_hash_compute()...");
    psa_status = psa_hash_compute(PSA_ALG_SHA_512, test1_input, test1_input_len,
                                  psa_output, sizeof(psa_output), &psa_output_len);
    ESP_LOGI(TAG, "PSA status: 0x%x, output_len: %zu", (unsigned int)psa_status, psa_output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    TEST_ASSERT_EQUAL(64, psa_output_len);

    ESP_LOGI(TAG, "PSA result:      %02x %02x %02x %02x %02x %02x %02x %02x...",
             psa_output[0], psa_output[1], psa_output[2], psa_output[3],
             psa_output[4], psa_output[5], psa_output[6], psa_output[7]);
    ESP_LOGI(TAG, "Expected result: %02x %02x %02x %02x %02x %02x %02x %02x...",
             test1_expected[0], test1_expected[1], test1_expected[2], test1_expected[3],
             test1_expected[4], test1_expected[5], test1_expected[6], test1_expected[7]);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(test1_expected, psa_output, 64);
    ESP_LOGI(TAG, "✓ PSA SHA-512(\"abc\") PASSED");

    // Test with mbedtls_md
    ESP_LOGI(TAG, "Testing mbedtls_md()...");
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA512);
    TEST_ASSERT_NOT_NULL(md_info);

    mbedtls_ret = mbedtls_md(md_info, test1_input, test1_input_len, mbedtls_output);
    ESP_LOGI(TAG, "mbedtls_md return: %d", mbedtls_ret);
    TEST_ASSERT_EQUAL(0, mbedtls_ret);

    ESP_LOGI(TAG, "mbedtls result:  %02x %02x %02x %02x %02x %02x %02x %02x...",
             mbedtls_output[0], mbedtls_output[1], mbedtls_output[2], mbedtls_output[3],
             mbedtls_output[4], mbedtls_output[5], mbedtls_output[6], mbedtls_output[7]);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(test1_expected, mbedtls_output, 64);
    ESP_LOGI(TAG, "✓ mbedtls_md SHA-512(\"abc\") PASSED");

    // Verify both methods produce the same result
    TEST_ASSERT_EQUAL_MEMORY(psa_output, mbedtls_output, 64);
    ESP_LOGI(TAG, "✓ PSA and mbedtls_md results match");

    ESP_LOGI(TAG, "=== Test 2: SHA-512(\"\") (empty string) ===");

    // Test with PSA
    psa_status = psa_hash_compute(PSA_ALG_SHA_512, test2_input, test2_input_len,
                                  psa_output, sizeof(psa_output), &psa_output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    TEST_ASSERT_EQUAL(64, psa_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(test2_expected, psa_output, 64);
    ESP_LOGI(TAG, "✓ PSA SHA-512(\"\") PASSED");

    // Test with mbedtls_md
    mbedtls_ret = mbedtls_md(md_info, test2_input, test2_input_len, mbedtls_output);
    TEST_ASSERT_EQUAL(0, mbedtls_ret);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(test2_expected, mbedtls_output, 64);
    ESP_LOGI(TAG, "✓ mbedtls_md SHA-512(\"\") PASSED");

    // Verify both methods produce the same result
    TEST_ASSERT_EQUAL_MEMORY(psa_output, mbedtls_output, 64);
    ESP_LOGI(TAG, "✓ All PSA SHA-512 tests PASSED!");
}

TEST_CASE("Test PSA SHA-256 with known test vectors", "[hw_crypto][psa]")
{
    ESP_LOGI(TAG, "Testing PSA SHA-256 implementation with known test vectors");

    // Test Vector 1: SHA-256("abc")
    // Expected: ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
    const unsigned char test1_input[] = "abc";
    const size_t test1_input_len = 3;
    const unsigned char test1_expected[32] = {
        0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
        0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
        0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
        0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
    };

    // Test Vector 2: SHA-256("")
    // Expected: e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
    const unsigned char test2_input[] = "";
    const size_t test2_input_len = 0;
    const unsigned char test2_expected[32] = {
        0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,
        0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
        0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
        0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55
    };

    // Test Vector 3: SHA-256("hello world")
    // Expected: b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9
    const unsigned char test3_input[] = "hello world";
    const unsigned char test3_expected[32] = {
        0xb9, 0x4d, 0x27, 0xb9, 0x93, 0x4d, 0x3e, 0x08,
        0xa5, 0x2e, 0x52, 0xd7, 0xda, 0x7d, 0xab, 0xfa,
        0xc4, 0x84, 0xef, 0xe3, 0x7a, 0x53, 0x80, 0xee,
        0x90, 0x88, 0xf7, 0xac, 0xe2, 0xef, 0xcd, 0xe9
    };

    unsigned char psa_output[32];
    unsigned char mbedtls_output[32];
    size_t psa_output_len;
    psa_status_t psa_status;
    int mbedtls_ret;

    ESP_LOGI(TAG, "=== Test 1: SHA-256(\"abc\") ===");

    // Test with PSA
    ESP_LOGI(TAG, "Testing PSA psa_hash_compute()...");
    psa_status = psa_hash_compute(PSA_ALG_SHA_256, test1_input, test1_input_len,
                                  psa_output, sizeof(psa_output), &psa_output_len);
    ESP_LOGI(TAG, "PSA status: 0x%x, output_len: %zu", (unsigned int)psa_status, psa_output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    TEST_ASSERT_EQUAL(32, psa_output_len);

    ESP_LOGI(TAG, "PSA result:      %02x %02x %02x %02x %02x %02x %02x %02x...",
             psa_output[0], psa_output[1], psa_output[2], psa_output[3],
             psa_output[4], psa_output[5], psa_output[6], psa_output[7]);
    ESP_LOGI(TAG, "Expected result: %02x %02x %02x %02x %02x %02x %02x %02x...",
             test1_expected[0], test1_expected[1], test1_expected[2], test1_expected[3],
             test1_expected[4], test1_expected[5], test1_expected[6], test1_expected[7]);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(test1_expected, psa_output, 32);
    ESP_LOGI(TAG, "✓ PSA SHA-256(\"abc\") PASSED");

    // Test with mbedtls_md
    ESP_LOGI(TAG, "Testing mbedtls_md()...");
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    TEST_ASSERT_NOT_NULL(md_info);

    mbedtls_ret = mbedtls_md(md_info, test1_input, test1_input_len, mbedtls_output);
    ESP_LOGI(TAG, "mbedtls_md return: %d", mbedtls_ret);
    TEST_ASSERT_EQUAL(0, mbedtls_ret);

    ESP_LOGI(TAG, "mbedtls result:  %02x %02x %02x %02x %02x %02x %02x %02x...",
             mbedtls_output[0], mbedtls_output[1], mbedtls_output[2], mbedtls_output[3],
             mbedtls_output[4], mbedtls_output[5], mbedtls_output[6], mbedtls_output[7]);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(test1_expected, mbedtls_output, 32);
    ESP_LOGI(TAG, "✓ mbedtls_md SHA-256(\"abc\") PASSED");

    // Verify both methods produce the same result
    TEST_ASSERT_EQUAL_MEMORY(psa_output, mbedtls_output, 32);
    ESP_LOGI(TAG, "✓ PSA and mbedtls_md results match");

    ESP_LOGI(TAG, "=== Test 2: SHA-256(\"\") (empty string) ===");

    // Test with PSA
    psa_status = psa_hash_compute(PSA_ALG_SHA_256, test2_input, test2_input_len,
                                  psa_output, sizeof(psa_output), &psa_output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    TEST_ASSERT_EQUAL(32, psa_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(test2_expected, psa_output, 32);
    ESP_LOGI(TAG, "✓ PSA SHA-256(\"\") PASSED");

    // Test with mbedtls_md
    mbedtls_ret = mbedtls_md(md_info, test2_input, test2_input_len, mbedtls_output);
    TEST_ASSERT_EQUAL(0, mbedtls_ret);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(test2_expected, mbedtls_output, 32);
    ESP_LOGI(TAG, "✓ mbedtls_md SHA-256(\"\") PASSED");

    // Verify both methods produce the same result
    TEST_ASSERT_EQUAL_MEMORY(psa_output, mbedtls_output, 32);
    ESP_LOGI(TAG, "✓ All PSA SHA-256 tests PASSED!");

    // Test Vector 3: SHA-256("hello world")
    // This will do with PSA only but _update will be called multiple time

    ESP_LOGI(TAG, "=== Test 3: SHA-256(\"hello world\") ===");
    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    psa_status = psa_hash_setup(&operation, PSA_ALG_SHA_256);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    psa_status = psa_hash_update(&operation, (const uint8_t *)test3_input, 5); // "hello"
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    psa_status = psa_hash_update(&operation, (const uint8_t *)(test3_input + 5), 6); // " world"
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    psa_status = psa_hash_finish(&operation, psa_output, sizeof(psa_output), &psa_output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    TEST_ASSERT_EQUAL(32, psa_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(test3_expected, psa_output, 32);
    ESP_LOGI(TAG, "✓ PSA SHA-256(\"hello world\") PASSED");
}

TEST_CASE("Test PSA SHA-384 with known test vectors", "[hw_crypto][psa]")
{
    ESP_LOGI(TAG, "Testing PSA SHA-384 implementation with known test vectors");

    // Test Vector 1: SHA-384("abc")
    // Expected: cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7
    const unsigned char test1_input[] = "abc";
    const size_t test1_input_len = 3;
    const unsigned char test1_expected[48] = {
        0xcb, 0x00, 0x75, 0x3f, 0x45, 0xa3, 0x5e, 0x8b,
        0xb5, 0xa0, 0x3d, 0x69, 0x9a, 0xc6, 0x50, 0x07,
        0x27, 0x2c, 0x32, 0xab, 0x0e, 0xde, 0xd1, 0x63,
        0x1a, 0x8b, 0x60, 0x5a, 0x43, 0xff, 0x5b, 0xed,
        0x80, 0x86, 0x07, 0x2b, 0xa1, 0xe7, 0xcc, 0x23,
        0x58, 0xba, 0xec, 0xa1, 0x34, 0xc8, 0x25, 0xa7
    };

    // Test Vector 2: SHA-384("")
    // Expected: 38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b
    const unsigned char test2_input[] = "";
    const size_t test2_input_len = 0;
    const unsigned char test2_expected[48] = {
        0x38, 0xb0, 0x60, 0xa7, 0x51, 0xac, 0x96, 0x38,
        0x4c, 0xd9, 0x32, 0x7e, 0xb1, 0xb1, 0xe3, 0x6a,
        0x21, 0xfd, 0xb7, 0x11, 0x14, 0xbe, 0x07, 0x43,
        0x4c, 0x0c, 0xc7, 0xbf, 0x63, 0xf6, 0xe1, 0xda,
        0x27, 0x4e, 0xde, 0xbf, 0xe7, 0x6f, 0x65, 0xfb,
        0xd5, 0x1a, 0xd2, 0xf1, 0x48, 0x98, 0xb9, 0x5b
    };

    // Test Vector 3: SHA-384("hello world")
    // Expected: fdbd8e75a67f29f701a4e040385e2e23986303ea10239211af907fcbb83578b3e417cb71ce646efd0819dd8c088de1bd
    const unsigned char test3_input[] = "hello world";
    const unsigned char test3_expected[48] = {
        0xfd, 0xbd, 0x8e, 0x75, 0xa6, 0x7f, 0x29, 0xf7,
        0x01, 0xa4, 0xe0, 0x40, 0x38, 0x5e, 0x2e, 0x23,
        0x98, 0x63, 0x03, 0xea, 0x10, 0x23, 0x92, 0x11,
        0xaf, 0x90, 0x7f, 0xcb, 0xb8, 0x35, 0x78, 0xb3,
        0xe4, 0x17, 0xcb, 0x71, 0xce, 0x64, 0x6e, 0xfd,
        0x08, 0x19, 0xdd, 0x8c, 0x08, 0x8d, 0xe1, 0xbd
    };

    unsigned char psa_output[48];
    unsigned char mbedtls_output[48];
    size_t psa_output_len;
    psa_status_t psa_status;
    int mbedtls_ret;

    ESP_LOGI(TAG, "=== Test 1: SHA-384(\"abc\") ===");

    // Test with PSA
    ESP_LOGI(TAG, "Testing PSA psa_hash_compute()...");
    psa_status = psa_hash_compute(PSA_ALG_SHA_384, test1_input, test1_input_len,
                                  psa_output, sizeof(psa_output), &psa_output_len);
    ESP_LOGI(TAG, "PSA status: 0x%x, output_len: %zu", (unsigned int)psa_status, psa_output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    TEST_ASSERT_EQUAL(48, psa_output_len);

    ESP_LOGI(TAG, "PSA result:      %02x %02x %02x %02x %02x %02x %02x %02x...",
             psa_output[0], psa_output[1], psa_output[2], psa_output[3],
             psa_output[4], psa_output[5], psa_output[6], psa_output[7]);
    ESP_LOGI(TAG, "Expected result: %02x %02x %02x %02x %02x %02x %02x %02x...",
             test1_expected[0], test1_expected[1], test1_expected[2], test1_expected[3],
             test1_expected[4], test1_expected[5], test1_expected[6], test1_expected[7]);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(test1_expected, psa_output, 48);
    ESP_LOGI(TAG, "✓ PSA SHA-384(\"abc\") PASSED");

    // Test with mbedtls_md
    ESP_LOGI(TAG, "Testing mbedtls_md()...");
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA384);
    TEST_ASSERT_NOT_NULL(md_info);

    mbedtls_ret = mbedtls_md(md_info, test1_input, test1_input_len, mbedtls_output);
    ESP_LOGI(TAG, "mbedtls_md return: %d", mbedtls_ret);
    TEST_ASSERT_EQUAL(0, mbedtls_ret);

    ESP_LOGI(TAG, "mbedtls result:  %02x %02x %02x %02x %02x %02x %02x %02x...",
             mbedtls_output[0], mbedtls_output[1], mbedtls_output[2], mbedtls_output[3],
             mbedtls_output[4], mbedtls_output[5], mbedtls_output[6], mbedtls_output[7]);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(test1_expected, mbedtls_output, 48);
    ESP_LOGI(TAG, "✓ mbedtls_md SHA-384(\"abc\") PASSED");

    // Verify both methods produce the same result
    TEST_ASSERT_EQUAL_MEMORY(psa_output, mbedtls_output, 48);
    ESP_LOGI(TAG, "✓ PSA and mbedtls_md results match");

    ESP_LOGI(TAG, "=== Test 2: SHA-384(\"\") (empty string) ===");

    // Test with PSA
    psa_status = psa_hash_compute(PSA_ALG_SHA_384, test2_input, test2_input_len,
                                  psa_output, sizeof(psa_output), &psa_output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    TEST_ASSERT_EQUAL(48, psa_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(test2_expected, psa_output, 48);
    ESP_LOGI(TAG, "✓ PSA SHA-384(\"\") PASSED");

    // Test with mbedtls_md
    mbedtls_ret = mbedtls_md(md_info, test2_input, test2_input_len, mbedtls_output);
    TEST_ASSERT_EQUAL(0, mbedtls_ret);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(test2_expected, mbedtls_output, 48);
    ESP_LOGI(TAG, "✓ mbedtls_md SHA-384(\"\") PASSED");

    // Verify both methods produce the same result
    TEST_ASSERT_EQUAL_MEMORY(psa_output, mbedtls_output, 48);
    ESP_LOGI(TAG, "✓ All PSA SHA-384 tests PASSED!");

    // Test Vector 3: SHA-384("hello world")
    // This will do with PSA only but _update will be called multiple time

    ESP_LOGI(TAG, "=== Test 3: SHA-384(\"hello world\") ===");
    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    psa_status = psa_hash_setup(&operation, PSA_ALG_SHA_384);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    psa_status = psa_hash_update(&operation, (const uint8_t *)test3_input, 5); // "hello"
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    psa_status = psa_hash_update(&operation, (const uint8_t *)(test3_input + 5), 6); // " world"
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    psa_status = psa_hash_finish(&operation, psa_output, sizeof(psa_output), &psa_output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    TEST_ASSERT_EQUAL(48, psa_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(test3_expected, psa_output, 48);
    ESP_LOGI(TAG, "✓ PSA SHA-384(\"hello world\") PASSED");
}

TEST_CASE("Test PSA SHA-384 with clone", "[hw_crypto][psa]")
{
    // Test Vector 1: SHA-384("hello world")
    // Expected: fdbd8e75a67f29f701a4e040385e2e23986303ea10239211af907fcbb83578b3e417cb71ce646efd0819dd8c088de1bd
    const unsigned char test3_input[] = "hello world";
    const unsigned char test3_expected[48] = {
        0xfd, 0xbd, 0x8e, 0x75, 0xa6, 0x7f, 0x29, 0xf7,
        0x01, 0xa4, 0xe0, 0x40, 0x38, 0x5e, 0x2e, 0x23,
        0x98, 0x63, 0x03, 0xea, 0x10, 0x23, 0x92, 0x11,
        0xaf, 0x90, 0x7f, 0xcb, 0xb8, 0x35, 0x78, 0xb3,
        0xe4, 0x17, 0xcb, 0x71, 0xce, 0x64, 0x6e, 0xfd,
        0x08, 0x19, 0xdd, 0x8c, 0x08, 0x8d, 0xe1, 0xbd
    };

    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    psa_status_t psa_status = psa_hash_setup(&operation, PSA_ALG_SHA_384);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    psa_status = psa_hash_update(&operation, (const uint8_t *)test3_input, 5); // "hello"
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);

    psa_hash_operation_t clone = PSA_HASH_OPERATION_INIT;
    psa_status = psa_hash_clone(&operation, &clone);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    psa_status = psa_hash_update(&clone, (const uint8_t *)(test3_input + 5), 6); // " world"
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    unsigned char psa_output[48];
    size_t psa_output_len;
    psa_status = psa_hash_finish(&clone, psa_output, sizeof(psa_output), &psa_output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    TEST_ASSERT_EQUAL(48, psa_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(test3_expected, psa_output, 48);
    ESP_LOGI(TAG, "✓ PSA SHA-384(\"hello world\") with original PASSED");
}
