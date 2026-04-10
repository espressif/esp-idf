/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "psa/crypto.h"
#include "esp_types.h"
#include "esp_log.h"
#include "ccomp_timer.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_heap_caps.h"
#include "test_mbedtls_utils.h"

#define TAG "md5_test"
#define MD5_HASH_LEN 16

TEST_CASE("Test PSA MD5 with known test vectors", "[hw_crypto][psa]")
{
    ESP_LOGI(TAG, "Testing MD5 implementation with known test vectors");

    uint8_t input[128] = {0};
    for (int i = 0; i < 128; i++) {
        input[i] = i;
    }

    uint8_t hash_output[MD5_HASH_LEN] = {0};

    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;

    size_t hash_len = 0;

    status = psa_hash_setup(&operation, PSA_ALG_MD5);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS);
    status = psa_hash_update(&operation, input, sizeof(input));
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS);
    status = psa_hash_finish(&operation, hash_output, MD5_HASH_LEN, &hash_len);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS);

    uint8_t expected_hash[MD5_HASH_LEN] = {
        0x37, 0xEF, 0xF0, 0x18, 0x66, 0xBA, 0x3F, 0x53, 0x84, 0x21, 0xB3, 0x0B, 0x7C, 0xBE, 0xFC, 0xAC
    };

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_hash, hash_output, MD5_HASH_LEN);
}

TEST_CASE("Test PSA MD5 error cases", "[hw_crypto][psa]")
{
    ESP_LOGI(TAG, "Testing MD5 error handling");

    uint8_t input[64] = {0};
    uint8_t hash_output[MD5_HASH_LEN] = {0};
    size_t hash_len = 0;
    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    psa_status_t status;

    // Test 1: NULL hash buffer in hash_compute
    status = psa_hash_compute(PSA_ALG_MD5, input, sizeof(input), NULL, MD5_HASH_LEN, &hash_len);
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

    // Test 2: NULL input with non-zero length in hash_compute
    status = psa_hash_compute(PSA_ALG_MD5, NULL, 10, hash_output, MD5_HASH_LEN, &hash_len);
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

    // Test 3: Buffer too small in hash_compute
    status = psa_hash_compute(PSA_ALG_MD5, input, sizeof(input), hash_output, MD5_HASH_LEN - 1, &hash_len);
    TEST_ASSERT_EQUAL(PSA_ERROR_BUFFER_TOO_SMALL, status);

    // Test 4: NULL input with non-zero length in hash_update
    operation = psa_hash_operation_init();
    status = psa_hash_setup(&operation, PSA_ALG_MD5);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    status = psa_hash_update(&operation, NULL, 10);
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);
    psa_hash_abort(&operation);

    // Test 5: NULL hash buffer in hash_finish
    operation = psa_hash_operation_init();
    status = psa_hash_setup(&operation, PSA_ALG_MD5);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    status = psa_hash_finish(&operation, NULL, MD5_HASH_LEN, &hash_len);
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);
    psa_hash_abort(&operation);

    // Test 6: Buffer too small in hash_finish
    operation = psa_hash_operation_init();
    status = psa_hash_setup(&operation, PSA_ALG_MD5);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    status = psa_hash_update(&operation, input, sizeof(input));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    status = psa_hash_finish(&operation, hash_output, 8, &hash_len);
    TEST_ASSERT_EQUAL(PSA_ERROR_BUFFER_TOO_SMALL, status);
    psa_hash_abort(&operation);

    // Test 7: Multiple aborts should be safe
    operation = psa_hash_operation_init();
    status = psa_hash_setup(&operation, PSA_ALG_MD5);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    status = psa_hash_abort(&operation);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    status = psa_hash_abort(&operation);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    ESP_LOGI(TAG, "All MD5 error cases passed");
}

TEST_CASE("Test PSA MD5 clone operation", "[hw_crypto][psa]")
{
    ESP_LOGI(TAG, "Testing MD5 clone operation");

    uint8_t input_part1[32];
    uint8_t input_part2[32];
    for (int i = 0; i < 32; i++) {
        input_part1[i] = i;
        input_part2[i] = i + 32;
    }

    uint8_t hash_output1[MD5_HASH_LEN] = {0};
    uint8_t hash_output2[MD5_HASH_LEN] = {0};
    size_t hash_len1 = 0, hash_len2 = 0;
    psa_hash_operation_t operation1 = PSA_HASH_OPERATION_INIT;
    psa_hash_operation_t operation2 = PSA_HASH_OPERATION_INIT;
    psa_status_t status;

    // Setup and update first operation
    status = psa_hash_setup(&operation1, PSA_ALG_MD5);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    status = psa_hash_update(&operation1, input_part1, sizeof(input_part1));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Clone the operation
    status = psa_hash_clone(&operation1, &operation2);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Continue both operations with the same data
    status = psa_hash_update(&operation1, input_part2, sizeof(input_part2));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    status = psa_hash_update(&operation2, input_part2, sizeof(input_part2));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Finish both operations
    status = psa_hash_finish(&operation1, hash_output1, MD5_HASH_LEN, &hash_len1);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    status = psa_hash_finish(&operation2, hash_output2, MD5_HASH_LEN, &hash_len2);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Both should produce the same hash
    TEST_ASSERT_EQUAL_HEX8_ARRAY(hash_output1, hash_output2, MD5_HASH_LEN);

    ESP_LOGI(TAG, "MD5 clone operation test passed");
}

TEST_CASE("Test PSA MD5 with zero-length input", "[hw_crypto][psa]")
{
    ESP_LOGI(TAG, "Testing MD5 with zero-length input");

    uint8_t hash_output[MD5_HASH_LEN] = {0};
    size_t hash_len = 0;
    psa_status_t status;

    // Empty string MD5 hash: d41d8cd98f00b204e9800998ecf8427e
    uint8_t expected_hash[MD5_HASH_LEN] = {
        0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,
        0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42, 0x7E
    };

    // Test with hash_compute
    status = psa_hash_compute(PSA_ALG_MD5, NULL, 0, hash_output, MD5_HASH_LEN, &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    TEST_ASSERT_EQUAL(MD5_HASH_LEN, hash_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_hash, hash_output, MD5_HASH_LEN);

    // Test with hash_setup/update/finish
    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    memset(hash_output, 0, MD5_HASH_LEN);

    status = psa_hash_setup(&operation, PSA_ALG_MD5);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    status = psa_hash_update(&operation, NULL, 0);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    status = psa_hash_finish(&operation, hash_output, MD5_HASH_LEN, &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    TEST_ASSERT_EQUAL(MD5_HASH_LEN, hash_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_hash, hash_output, MD5_HASH_LEN);

    ESP_LOGI(TAG, "Zero-length input test passed");
}

TEST_CASE("Test PSA MD5 incremental updates", "[hw_crypto][psa]")
{
    ESP_LOGI(TAG, "Testing MD5 incremental updates");

    uint8_t input[128];
    for (int i = 0; i < 128; i++) {
        input[i] = i;
    }

    uint8_t hash_full[MD5_HASH_LEN] = {0};
    uint8_t hash_incremental[MD5_HASH_LEN] = {0};
    size_t hash_len = 0;
    psa_status_t status;

    // Compute hash in one go
    status = psa_hash_compute(PSA_ALG_MD5, input, sizeof(input), hash_full, MD5_HASH_LEN, &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Compute hash incrementally in small chunks
    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    status = psa_hash_setup(&operation, PSA_ALG_MD5);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    for (int i = 0; i < 128; i += 16) {
        status = psa_hash_update(&operation, input + i, 16);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    }

    status = psa_hash_finish(&operation, hash_incremental, MD5_HASH_LEN, &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Both methods should produce the same hash
    TEST_ASSERT_EQUAL_HEX8_ARRAY(hash_full, hash_incremental, MD5_HASH_LEN);

    ESP_LOGI(TAG, "Incremental updates test passed");
}

TEST_CASE("psa MD5 performance", "[mbedtls]")
{
    const unsigned CALLS = 256;
    const unsigned CALL_SZ = 16 * 1024;
    float elapsed_usec;
    uint8_t hash_output[MD5_HASH_LEN] = {0};

    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    psa_status_t status = psa_hash_setup(&operation, PSA_ALG_MD5);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // allocate internal memory
    uint8_t *buf = heap_caps_malloc(CALL_SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buf);
    memset(buf, 0x55, CALL_SZ);

    ccomp_timer_start();
    for (int c = 0; c < CALLS; c++) {
        status = psa_hash_update(&operation, buf, CALL_SZ);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    }
    size_t hash_length;
    status = psa_hash_finish(&operation, hash_output, MD5_HASH_LEN, &hash_length);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    elapsed_usec = ccomp_timer_stop();
    free(buf);

    const char *expected_hash = "9a9819e47b2014589ae2057458664df7";
    char hash_str[MD5_HASH_LEN * 2 + 1];
    utils_bin2hex(hash_str, sizeof(hash_str), hash_output, MD5_HASH_LEN);

    TEST_ASSERT_EQUAL_STRING(expected_hash, hash_str);

    float mb_sec = (CALL_SZ * CALLS) / elapsed_usec;
    printf("MD5 rate %.3fMB/sec\n", mb_sec);
}
