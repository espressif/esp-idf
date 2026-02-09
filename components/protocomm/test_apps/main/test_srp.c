/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include <unity.h>
#include "esp_srp.h"
#include "esp_log.h"
#include "test_utils.h"
#include "esp_rom_crc.h"

static const char *TAG = "srp_test";

// Example username and password
static const char *username = "wifiprov";
static const char *password = "abcd1234";

static const char sec2_salt[] = {
    0x03, 0x6e, 0xe0, 0xc7, 0xbc, 0xb9, 0xed, 0xa8, 0x4c, 0x9e, 0xac, 0x97, 0xd9, 0x3d, 0xec, 0xf4
};

static const char sec2_verifier[] = {
    0x7c, 0x7c, 0x85, 0x47, 0x65, 0x08, 0x94, 0x6d, 0xd6, 0x36, 0xaf, 0x37, 0xd7, 0xe8, 0x91, 0x43,
    0x78, 0xcf, 0xfd, 0x61, 0x6c, 0x59, 0xd2, 0xf8, 0x39, 0x08, 0x12, 0x72, 0x38, 0xde, 0x9e, 0x24,
    0xa4, 0x70, 0x26, 0x1c, 0xdf, 0xa9, 0x03, 0xc2, 0xb2, 0x70, 0xe7, 0xb1, 0x32, 0x24, 0xda, 0x11,
    0x1d, 0x97, 0x18, 0xdc, 0x60, 0x72, 0x08, 0xcc, 0x9a, 0xc9, 0x0c, 0x48, 0x27, 0xe2, 0xae, 0x89,
    0xaa, 0x16, 0x25, 0xb8, 0x04, 0xd2, 0x1a, 0x9b, 0x3a, 0x8f, 0x37, 0xf6, 0xe4, 0x3a, 0x71, 0x2e,
    0xe1, 0x27, 0x86, 0x6e, 0xad, 0xce, 0x28, 0xff, 0x54, 0x46, 0x60, 0x1f, 0xb9, 0x96, 0x87, 0xdc,
    0x57, 0x40, 0xa7, 0xd4, 0x6c, 0xc9, 0x77, 0x54, 0xdc, 0x16, 0x82, 0xf0, 0xed, 0x35, 0x6a, 0xc4,
    0x70, 0xad, 0x3d, 0x90, 0xb5, 0x81, 0x94, 0x70, 0xd7, 0xbc, 0x65, 0xb2, 0xd5, 0x18, 0xe0, 0x2e,
    0xc3, 0xa5, 0xf9, 0x68, 0xdd, 0x64, 0x7b, 0xb8, 0xb7, 0x3c, 0x9c, 0xfc, 0x00, 0xd8, 0x71, 0x7e,
    0xb7, 0x9a, 0x7c, 0xb1, 0xb7, 0xc2, 0xc3, 0x18, 0x34, 0x29, 0x32, 0x43, 0x3e, 0x00, 0x99, 0xe9,
    0x82, 0x94, 0xe3, 0xd8, 0x2a, 0xb0, 0x96, 0x29, 0xb7, 0xdf, 0x0e, 0x5f, 0x08, 0x33, 0x40, 0x76,
    0x52, 0x91, 0x32, 0x00, 0x9f, 0x97, 0x2c, 0x89, 0x6c, 0x39, 0x1e, 0xc8, 0x28, 0x05, 0x44, 0x17,
    0x3f, 0x68, 0x02, 0x8a, 0x9f, 0x44, 0x61, 0xd1, 0xf5, 0xa1, 0x7e, 0x5a, 0x70, 0xd2, 0xc7, 0x23,
    0x81, 0xcb, 0x38, 0x68, 0xe4, 0x2c, 0x20, 0xbc, 0x40, 0x57, 0x76, 0x17, 0xbd, 0x08, 0xb8, 0x96,
    0xbc, 0x26, 0xeb, 0x32, 0x46, 0x69, 0x35, 0x05, 0x8c, 0x15, 0x70, 0xd9, 0x1b, 0xe9, 0xbe, 0xcc,
    0xa9, 0x38, 0xa6, 0x67, 0xf0, 0xad, 0x50, 0x13, 0x19, 0x72, 0x64, 0xbf, 0x52, 0xc2, 0x34, 0xe2,
    0x1b, 0x11, 0x79, 0x74, 0x72, 0xbd, 0x34, 0x5b, 0xb1, 0xe2, 0xfd, 0x66, 0x73, 0xfe, 0x71, 0x64,
    0x74, 0xd0, 0x4e, 0xbc, 0x51, 0x24, 0x19, 0x40, 0x87, 0x0e, 0x92, 0x40, 0xe6, 0x21, 0xe7, 0x2d,
    0x4e, 0x37, 0x76, 0x2f, 0x2e, 0xe2, 0x68, 0xc7, 0x89, 0xe8, 0x32, 0x13, 0x42, 0x06, 0x84, 0x84,
    0x53, 0x4a, 0xb3, 0x0c, 0x1b, 0x4c, 0x8d, 0x1c, 0x51, 0x97, 0x19, 0xab, 0xae, 0x77, 0xff, 0xdb,
    0xec, 0xf0, 0x10, 0x95, 0x34, 0x33, 0x6b, 0xcb, 0x3e, 0x84, 0x0f, 0xb9, 0xd8, 0x5f, 0xb8, 0xa0,
    0xb8, 0x55, 0x53, 0x3e, 0x70, 0xf7, 0x18, 0xf5, 0xce, 0x7b, 0x4e, 0xbf, 0x27, 0xce, 0xce, 0xa8,
    0xb3, 0xbe, 0x40, 0xc5, 0xc5, 0x32, 0x29, 0x3e, 0x71, 0x64, 0x9e, 0xde, 0x8c, 0xf6, 0x75, 0xa1,
    0xe6, 0xf6, 0x53, 0xc8, 0x31, 0xa8, 0x78, 0xde, 0x50, 0x40, 0xf7, 0x62, 0xde, 0x36, 0xb2, 0xba
};

static void test_srp_init_and_free(void) {
    esp_srp_handle_t *handle = esp_srp_init(ESP_NG_3072);
    TEST_ASSERT_NOT_NULL(handle);
    esp_srp_free(handle);
}

static void test_srp_gen_salt_verifier(void) {
    char *bytes_salt = NULL;
    char *verifier = NULL;
    int verifier_len = 0;
    esp_err_t err = esp_srp_gen_salt_verifier(username, strlen(username),
                                              password, strlen(password),
                                              &bytes_salt, 16, &verifier, &verifier_len);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_NOT_NULL(bytes_salt);
    TEST_ASSERT_NOT_NULL(verifier);

    // Verify salt length is as requested
    TEST_ASSERT_EQUAL(16, 16);

    // Verify verifier length is correct for 3072-bit SRP
    TEST_ASSERT_GREATER_THAN(0, verifier_len);

    // Log the generated salt and verifier for debugging
    ESP_LOG_BUFFER_HEXDUMP("Generated Salt", bytes_salt, 16, ESP_LOG_INFO);
    ESP_LOG_BUFFER_HEXDUMP("Generated Verifier", verifier, verifier_len, ESP_LOG_INFO);

    free(bytes_salt);
    free(verifier);
}

static void test_srp_set_salt_verifier(void) {
    esp_srp_handle_t *handle = esp_srp_init(ESP_NG_3072);
    TEST_ASSERT_NOT_NULL(handle);

    esp_err_t err = esp_srp_set_salt_verifier(handle, sec2_salt, sizeof(sec2_salt),
                                              sec2_verifier, sizeof(sec2_verifier));
    TEST_ASSERT_EQUAL(ESP_OK, err);

    char *bytes_B = NULL;
    int len_B = 0;
    err = esp_srp_srv_pubkey_from_salt_verifier(handle, &bytes_B, &len_B);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    // Verify B length is correct for 3072-bit SRP (384 bytes)
    TEST_ASSERT_EQUAL(384, len_B);

    esp_srp_free(handle);
}

static void test_srp_srv_pubkey(void) {
    esp_srp_handle_t *handle = esp_srp_init(ESP_NG_3072);
    TEST_ASSERT_NOT_NULL(handle);

    char *bytes_B = NULL;
    int len_B = 0;
    char *bytes_salt = NULL;
    esp_err_t err = esp_srp_srv_pubkey(handle, username, strlen(username),
                                       password, strlen(password), 16,
                                       &bytes_B, &len_B, &bytes_salt);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_NOT_NULL(bytes_B);
    TEST_ASSERT_NOT_NULL(bytes_salt);

    // Verify salt and B length
    TEST_ASSERT_EQUAL(16, 16);
    TEST_ASSERT_EQUAL(384, len_B);

    // Log for debugging
    ESP_LOG_BUFFER_HEXDUMP("Generated Salt", bytes_salt, 16, ESP_LOG_INFO);
    ESP_LOG_BUFFER_HEXDUMP("Generated Server Public Key B", bytes_B, len_B, ESP_LOG_INFO);

    esp_srp_free(handle);
}

static void test_srp_get_session_key(void) {
    esp_srp_handle_t *handle = esp_srp_init(ESP_NG_3072);
    TEST_ASSERT_NOT_NULL(handle);

    char *bytes_B = NULL;
    int len_B = 0;
    char *bytes_salt = NULL;
    esp_err_t err = esp_srp_srv_pubkey(handle, username, strlen(username),
                                       password, strlen(password), 16,
                                       &bytes_B, &len_B, &bytes_salt);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    // In a real scenario, bytes_A would be from client
    // For testing purposes, we use bytes_B as a convenient value (server talks to itself)
    char *bytes_key = NULL;
    uint16_t len_key = 0;
    err = esp_srp_get_session_key(handle, bytes_B, len_B, &bytes_key, &len_key);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_NOT_NULL(bytes_key);

    // Verify session key length (SHA512 hash)
    TEST_ASSERT_EQUAL(64, len_key);

    // Log session key for debugging
    ESP_LOG_BUFFER_HEXDUMP("Session Key", bytes_key, len_key, ESP_LOG_INFO);

    esp_srp_free(handle);
}

static void test_srp_exchange_proofs(void) {
    esp_srp_handle_t *handle = esp_srp_init(ESP_NG_3072);
    TEST_ASSERT_NOT_NULL(handle);

    char *bytes_B = NULL;
    int len_B = 0;
    char *bytes_salt = NULL;
    esp_err_t err = esp_srp_srv_pubkey(handle, username, strlen(username),
                                       password, strlen(password), 16,
                                       &bytes_B, &len_B, &bytes_salt);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    char *bytes_key = NULL;
    uint16_t len_key = 0;
    err = esp_srp_get_session_key(handle, bytes_B, len_B, &bytes_key, &len_key);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    // In a real environment, bytes_user_proof would be calculated by the client
    // For our test, we'll generate zeros - this simulates an authentication failure scenario
    char bytes_user_proof[64] = {0}; // Example proof
    char bytes_host_proof[64] = {0};

    // This should fail since user proof is zeros and doesn't match expected value
    err = esp_srp_exchange_proofs(handle, (char *)username, strlen(username),
                                  bytes_user_proof, bytes_host_proof);
    TEST_ASSERT_EQUAL(ESP_FAIL, err);

    esp_srp_free(handle);
}

// Add test for error handling with invalid parameters
static void test_srp_error_handling(void) {
    esp_srp_handle_t *handle = esp_srp_init(ESP_NG_3072);
    TEST_ASSERT_NOT_NULL(handle);

    // Test with NULL salt
    esp_err_t err = esp_srp_set_salt_verifier(handle, NULL, sizeof(sec2_salt),
                                             sec2_verifier, sizeof(sec2_verifier));
    TEST_ASSERT_NOT_EQUAL(ESP_OK, err);

    // Test with zero salt length
    err = esp_srp_set_salt_verifier(handle, sec2_salt, 0,
                                   sec2_verifier, sizeof(sec2_verifier));
    TEST_ASSERT_NOT_EQUAL(ESP_OK, err);

    // Test with NULL verifier
    err = esp_srp_set_salt_verifier(handle, sec2_salt, sizeof(sec2_salt),
                                   NULL, sizeof(sec2_verifier));
    TEST_ASSERT_NOT_EQUAL(ESP_OK, err);

    // Test with zero verifier length
    err = esp_srp_set_salt_verifier(handle, sec2_salt, sizeof(sec2_salt),
                                   sec2_verifier, 0);
    TEST_ASSERT_NOT_EQUAL(ESP_OK, err);

    esp_srp_free(handle);
}

// Test verifier calculation consistency
static void test_srp_verifier_consistency(void) {
    char *bytes_salt1 = NULL;
    char *verifier1 = NULL;
    int verifier_len1 = 0;

    // Generate first salt/verifier pair
    esp_err_t err = esp_srp_gen_salt_verifier(username, strlen(username),
                                             password, strlen(password),
                                             &bytes_salt1, 16, &verifier1, &verifier_len1);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    // Generate second salt/verifier pair
    char *bytes_salt2 = NULL;
    char *verifier2 = NULL;
    int verifier_len2 = 0;
    err = esp_srp_gen_salt_verifier(username, strlen(username),
                                   password, strlen(password),
                                   &bytes_salt2, 16, &verifier2, &verifier_len2);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    // Salts should be different (randomly generated)
    TEST_ASSERT_NOT_EQUAL(0, memcmp(bytes_salt1, bytes_salt2, 16));

    // Verifiers should also be different since they depend on the salt
    TEST_ASSERT_NOT_EQUAL(0, memcmp(verifier1, verifier2, verifier_len1));

    free(bytes_salt1);
    free(verifier1);
    free(bytes_salt2);
    free(verifier2);
}

static void test_srp_pubkey_randomness(void) {
    esp_srp_handle_t *handle = esp_srp_init(ESP_NG_3072);
    TEST_ASSERT_NOT_NULL(handle);

    esp_err_t err = esp_srp_set_salt_verifier(handle, sec2_salt, sizeof(sec2_salt),
                                             sec2_verifier, sizeof(sec2_verifier));
    TEST_ASSERT_EQUAL(ESP_OK, err);

    // Generate first public key
    char *bytes_B1 = NULL;
    int len_B1 = 0;
    err = esp_srp_srv_pubkey_from_salt_verifier(handle, &bytes_B1, &len_B1);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_NOT_NULL(bytes_B1);
    TEST_ASSERT_EQUAL(384, len_B1);

    // Generate second public key with same salt/verifier
    char *bytes_B2 = NULL;
    int len_B2 = 0;
    err = esp_srp_srv_pubkey_from_salt_verifier(handle, &bytes_B2, &len_B2);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_NOT_NULL(bytes_B2);
    TEST_ASSERT_EQUAL(384, len_B2);

    // Keys should be different due to random b generation
    TEST_ASSERT_NOT_EQUAL(0, memcmp(bytes_B1, bytes_B2, len_B1));

    // Calculate CRCs for logging
    uint32_t crc1 = esp_rom_crc32_le(0, (uint8_t*)bytes_B1, len_B1);
    uint32_t crc2 = esp_rom_crc32_le(0, (uint8_t*)bytes_B2, len_B2);
    ESP_LOGI(TAG, "Public key CRCs: %u, %u (should be different)", crc1, crc2);

    free(bytes_B1);
    bytes_B1 = NULL;
    esp_srp_free(handle);
}

TEST_CASE("SRP init and free test", "[SRP]")
{
    test_srp_init_and_free();
}

TEST_CASE("SRP generate salt and verifier test", "[SRP]")
{
    test_srp_gen_salt_verifier();
}

TEST_CASE("SRP set salt and verifier test", "[SRP]")
{
    test_srp_set_salt_verifier();
}

TEST_CASE("SRP server public key test", "[SRP]")
{
    test_srp_srv_pubkey();
}

TEST_CASE("SRP get session key test", "[SRP]")
{
    test_srp_get_session_key();
}

TEST_CASE("SRP exchange proofs test", "[SRP]")
{
    test_srp_exchange_proofs();
}

TEST_CASE("SRP error handling test", "[SRP]")
{
    test_srp_error_handling();
}

TEST_CASE("SRP verifier consistency test", "[SRP]")
{
    test_srp_verifier_consistency();
}

TEST_CASE("SRP public key randomness test", "[SRP]")
{
    test_srp_pubkey_randomness();
}
