/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* PSA CMAC test
*/
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <esp_system.h>
#include "psa/crypto.h"
#include "unity.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "test_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_memory_utils.h"

#if CONFIG_MBEDTLS_CMAC_C
static const uint8_t key_128[] = {
    0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
    0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
};

static const uint8_t key_256[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
};

static const uint8_t test_data[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51
};

// Expected CMAC values from the mbedtls implementation
static const uint8_t expected_cmac_128[] = {
    0x93, 0xae, 0x18, 0x36, 0xdf, 0xbd, 0x91, 0x06,
    0xa5, 0xd1, 0x84, 0x5c, 0xe5, 0x61, 0x02, 0xe2,
};

static const uint8_t expected_cmac_256[] = {
    0x35, 0x17, 0x99, 0xb0, 0xfd, 0xb1, 0x5b, 0x47,
    0x98, 0xe3, 0x47, 0xef, 0xa3, 0xb4, 0xe1, 0x89,
};

static const uint8_t expected_cmac_zero_length[] = {
    0xd8, 0xa8, 0x58, 0x43, 0x62, 0xe9, 0x93, 0xf8,
    0xd5, 0x29, 0x24, 0xf6, 0x39, 0x07, 0xc4, 0x88,
};

TEST_CASE("PSA CMAC AES-128 test", "[psa_cmac]")
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&attributes, PSA_ALG_CMAC);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    // Import the key
    status = psa_import_key(&attributes, key_128, sizeof(key_128), &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Allocate internal memory for CMAC output
    uint8_t *cmac = heap_caps_malloc(PSA_MAC_LENGTH(PSA_KEY_TYPE_AES, 128, PSA_ALG_CMAC),
                                    MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(cmac);

    size_t cmac_length = 0;

    // Calculate CMAC
    status = psa_mac_compute(key_id, PSA_ALG_CMAC,
                           test_data, sizeof(test_data),
                           cmac, PSA_MAC_LENGTH(PSA_KEY_TYPE_AES, 128, PSA_ALG_CMAC),
                           &cmac_length);

    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    TEST_ASSERT_EQUAL(16, cmac_length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cmac_128, cmac, 16);

    // Verify CMAC
    status = psa_mac_verify(key_id, PSA_ALG_CMAC,
                          test_data, sizeof(test_data),
                          expected_cmac_128, sizeof(expected_cmac_128));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Cleanup
    psa_destroy_key(key_id);
    free(cmac);
}

TEST_CASE("PSA CMAC AES-256 test", "[psa_cmac]")
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&attributes, PSA_ALG_CMAC);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 256);

    // Import the key
    status = psa_import_key(&attributes, key_256, sizeof(key_256), &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Allocate internal memory for CMAC output
    uint8_t *cmac = heap_caps_malloc(PSA_MAC_LENGTH(PSA_KEY_TYPE_AES, 256, PSA_ALG_CMAC),
                                    MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(cmac);

    size_t cmac_length = 0;

    // Calculate CMAC
    status = psa_mac_compute(key_id, PSA_ALG_CMAC,
                           test_data, sizeof(test_data),
                           cmac, PSA_MAC_LENGTH(PSA_KEY_TYPE_AES, 256, PSA_ALG_CMAC),
                           &cmac_length);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    TEST_ASSERT_EQUAL(16, cmac_length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cmac_256, cmac, 16);

    // Verify CMAC
    status = psa_mac_verify(key_id, PSA_ALG_CMAC,
                          test_data, sizeof(test_data),
                          expected_cmac_256, sizeof(expected_cmac_256));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Cleanup
    psa_destroy_key(key_id);
    free(cmac);
}

TEST_CASE("PSA CMAC AES-128 multipart test", "[psa_cmac]")
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    psa_mac_operation_t operation = PSA_MAC_OPERATION_INIT;

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&attributes, PSA_ALG_CMAC);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    // Import the key
    status = psa_import_key(&attributes, key_128, sizeof(key_128), &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Allocate internal memory for CMAC output
    uint8_t *cmac = heap_caps_malloc(PSA_MAC_LENGTH(PSA_KEY_TYPE_AES, 128, PSA_ALG_CMAC),
                                    MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(cmac);

    size_t cmac_length = 0;

    // Test multipart operation with different chunk sizes
    for (size_t chunk_size = 1; chunk_size < sizeof(test_data); chunk_size++) {
        // Setup operation
        status = psa_mac_sign_setup(&operation, key_id, PSA_ALG_CMAC);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        // Process data in chunks
        for (size_t offset = 0; offset < sizeof(test_data); offset += chunk_size) {
            size_t current_chunk_size = (offset + chunk_size > sizeof(test_data)) ?
                                       (sizeof(test_data) - offset) : chunk_size;

            status = psa_mac_update(&operation, test_data + offset, current_chunk_size);
            TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        }

        // Finish operation
        status = psa_mac_sign_finish(&operation, cmac, PSA_MAC_LENGTH(PSA_KEY_TYPE_AES, 128, PSA_ALG_CMAC),
                                   &cmac_length);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        TEST_ASSERT_EQUAL(16, cmac_length);
        // Verify result
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cmac_128, cmac, 16);
    }

    // Cleanup
    psa_destroy_key(key_id);
    free(cmac);
}

TEST_CASE("PSA CMAC AES-128 multipart verify test", "[psa_cmac]")
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&attributes, PSA_ALG_CMAC);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    // Import the key
    status = psa_import_key(&attributes, key_128, sizeof(key_128), &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Allocate internal memory for CMAC output
    uint8_t *cmac = heap_caps_malloc(PSA_MAC_LENGTH(PSA_KEY_TYPE_AES, 128, PSA_ALG_CMAC),
                                    MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(cmac);

    size_t cmac_length = 0;

    status = psa_mac_compute(key_id, PSA_ALG_CMAC,
                           test_data, sizeof(test_data),
                           cmac, PSA_MAC_LENGTH(PSA_KEY_TYPE_AES, 128, PSA_ALG_CMAC),
                           &cmac_length);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    TEST_ASSERT_EQUAL(16, cmac_length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cmac_128, cmac, 16);

    // Verify CMAC multipart
    psa_mac_operation_t verify_operation = PSA_MAC_OPERATION_INIT;
    status = psa_mac_verify_setup(&verify_operation, key_id, PSA_ALG_CMAC);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_mac_update(&verify_operation, test_data, sizeof(test_data));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_mac_verify_finish(&verify_operation, cmac, cmac_length);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Modify one byte of cmac and check for failure
    cmac[0] = cmac[0] + 1;

    status = psa_mac_verify_setup(&verify_operation, key_id, PSA_ALG_CMAC);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_mac_update(&verify_operation, test_data, sizeof(test_data));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_mac_verify_finish(&verify_operation, cmac, cmac_length);
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_SIGNATURE, status);

    // Cleanup
    psa_destroy_key(key_id);
    free(cmac);
}

TEST_CASE("PSA CMAC zero-length test", "[psa_cmac]")
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&attributes, PSA_ALG_CMAC);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    // Import the key
    status = psa_import_key(&attributes, key_128, sizeof(key_128), &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Allocate internal memory for CMAC output
    uint8_t *cmac = heap_caps_malloc(PSA_MAC_LENGTH(PSA_KEY_TYPE_AES, 128, PSA_ALG_CMAC),
                                    MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(cmac);

    size_t cmac_length = 0;

    // Calculate CMAC on zero-length data
    status = psa_mac_compute(key_id, PSA_ALG_CMAC,
                           NULL, 0,
                           cmac, PSA_MAC_LENGTH(PSA_KEY_TYPE_AES, 128, PSA_ALG_CMAC),
                           &cmac_length);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    TEST_ASSERT_EQUAL(16, cmac_length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cmac_zero_length, cmac, 16);

    // Verify CMAC
    status = psa_mac_verify(key_id, PSA_ALG_CMAC,
                          NULL, 0,
                          expected_cmac_zero_length, sizeof(expected_cmac_zero_length));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Cleanup
    psa_destroy_key(key_id);
    free(cmac);
}

TEST_CASE("PSA CMAC memory alignment test", "[psa_cmac]")
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&attributes, PSA_ALG_CMAC);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    // Import the key
    status = psa_import_key(&attributes, key_128, sizeof(key_128), &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Allocate memory with different capabilities
    uint8_t *cmac_internal = heap_caps_malloc(16, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    uint8_t *cmac_dma = heap_caps_malloc(16, MALLOC_CAP_DMA|MALLOC_CAP_8BIT);

    TEST_ASSERT_NOT_NULL(cmac_internal);
    TEST_ASSERT_NOT_NULL(cmac_dma);

    size_t cmac_length_internal = 0;
    size_t cmac_length_dma = 0;

    // Calculate CMAC with internal memory
    status = psa_mac_compute(key_id, PSA_ALG_CMAC,
                           test_data, sizeof(test_data),
                           cmac_internal, 16,
                           &cmac_length_internal);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    TEST_ASSERT_EQUAL(16, cmac_length_internal);

    // Calculate CMAC with DMA-capable memory
    status = psa_mac_compute(key_id, PSA_ALG_CMAC,
                           test_data, sizeof(test_data),
                           cmac_dma, 16,
                           &cmac_length_dma);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    TEST_ASSERT_EQUAL(16, cmac_length_dma);

    // Results should be identical
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cmac_128, cmac_internal, 16);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cmac_128, cmac_dma, 16);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cmac_internal, cmac_dma, 16);

    // Cleanup
    psa_destroy_key(key_id);
    free(cmac_internal);
    free(cmac_dma);
}

TEST_CASE("PSA CMAC verify failure test", "[psa_cmac]")
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&attributes, PSA_ALG_CMAC);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    // Import the key
    status = psa_import_key(&attributes, key_128, sizeof(key_128), &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Create an invalid CMAC by modifying one byte
    uint8_t invalid_cmac[16];
    memcpy(invalid_cmac, expected_cmac_128, 16);
    invalid_cmac[0] ^= 0x01;  // Flip one bit

    // Verify should fail with the modified CMAC
    status = psa_mac_verify(key_id, PSA_ALG_CMAC,
                          test_data, sizeof(test_data),
                          invalid_cmac, sizeof(invalid_cmac));
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_SIGNATURE, status);

    // Verify should succeed with the correct CMAC
    status = psa_mac_verify(key_id, PSA_ALG_CMAC,
                          test_data, sizeof(test_data),
                          expected_cmac_128, sizeof(expected_cmac_128));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Cleanup
    psa_destroy_key(key_id);
}
#endif /* CONFIG_MBEDTLS_CMAC_C */
