/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* PSA HMAC test
*/

#include "psa/crypto.h"
#include "unity.h"
#include "esp_log.h"

static const uint8_t key_256[] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
};

static const uint8_t test_data[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51
};

static const uint8_t expected_hmac_sha1[] = {
    0xb7, 0xb4, 0x9b, 0xc1, 0x8a, 0x88, 0x43, 0xa2,
    0xbb, 0x34, 0xdf, 0x27, 0xfe, 0x6c, 0x9b, 0x1b,
    0xbf, 0x4b, 0x57, 0xb5,
};

static const uint8_t expected_hmac_sha256[] = {
    0xc4, 0x45, 0xa1, 0xfc, 0x3f, 0x7e, 0x1b, 0xfc,
    0x4a, 0x33, 0x19, 0xfc, 0x68, 0xa5, 0x8e, 0xf1,
    0x81, 0x0f, 0x99, 0x70, 0x32, 0x06, 0x2c, 0xcb,
    0x50, 0xd7, 0x15, 0x78, 0x82, 0x10, 0xbe, 0xc6,
};

// Helper function to set up key attributes for HMAC
static void setup_hmac_key_attributes(psa_key_attributes_t *attributes,
                                       psa_algorithm_t alg,
                                       psa_key_lifetime_t lifetime)
{
    psa_set_key_usage_flags(attributes, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(attributes, alg);
    psa_set_key_type(attributes, PSA_KEY_TYPE_HMAC);
    psa_set_key_bits(attributes, 256);
    psa_set_key_lifetime(attributes, lifetime);
}

// Helper function to perform HMAC compute and verify (single-shot)
static void test_hmac_compute_and_verify(psa_key_id_t key_id,
                                          psa_algorithm_t alg,
                                          const uint8_t *data,
                                          size_t data_len,
                                          const uint8_t *expected_mac,
                                          size_t expected_mac_len)
{
    psa_status_t status;
    size_t hmac_length = PSA_HASH_LENGTH(alg);
    uint8_t *hmac = malloc(hmac_length);
    TEST_ASSERT_NOT_NULL(hmac);

    size_t mac_length = 0;
    status = psa_mac_compute(key_id, alg, data, data_len,
                             hmac, hmac_length, &mac_length);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_mac_verify(key_id, alg, data, data_len,
                            expected_mac, expected_mac_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    free(hmac);
}

// Helper function to perform HMAC multipart sign operation
static void test_hmac_multipart_sign(psa_mac_operation_t *op,
                                      psa_key_id_t key_id,
                                      psa_algorithm_t alg,
                                      const uint8_t *data,
                                      size_t data_len,
                                      uint8_t *mac,
                                      size_t mac_size,
                                      size_t *mac_len)
{
    psa_status_t status;

    status = psa_mac_sign_setup(op, key_id, alg);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_mac_update(op, data, data_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_mac_sign_finish(op, mac, mac_size, mac_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
}

// Helper function to perform HMAC multipart verify operation
static void test_hmac_multipart_verify(psa_mac_operation_t *op,
                                        psa_key_id_t key_id,
                                        psa_algorithm_t alg,
                                        const uint8_t *data,
                                        size_t data_len,
                                        const uint8_t *mac,
                                        size_t mac_len)
{
    psa_status_t status;

    status = psa_mac_verify_setup(op, key_id, alg);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_mac_update(op, data, data_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_mac_verify_finish(op, mac, mac_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
}

TEST_CASE("PSA HMAC SHA-1 test", "[psa_hmac]")
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    psa_algorithm_t alg = PSA_ALG_HMAC(PSA_ALG_SHA_1);

    setup_hmac_key_attributes(&attributes, alg, PSA_KEY_LIFETIME_VOLATILE);

    status = psa_import_key(&attributes, key_256, sizeof(key_256), &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    test_hmac_compute_and_verify(key_id, alg, test_data, sizeof(test_data),
                                 expected_hmac_sha1, sizeof(expected_hmac_sha1));

    psa_destroy_key(key_id);
    psa_reset_key_attributes(&attributes);
}

TEST_CASE("PSA HMAC SHA-256 test", "[psa_hmac]")
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    psa_algorithm_t alg = PSA_ALG_HMAC(PSA_ALG_SHA_256);

    setup_hmac_key_attributes(&attributes, alg, PSA_KEY_LIFETIME_VOLATILE);

    status = psa_import_key(&attributes, key_256, sizeof(key_256), &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    test_hmac_compute_and_verify(key_id, alg, test_data, sizeof(test_data),
                                 expected_hmac_sha256, sizeof(expected_hmac_sha256));

    psa_destroy_key(key_id);
    psa_reset_key_attributes(&attributes);
}

TEST_CASE("PSA HMAC SHA-256 multipart test", "[psa_hmac]")
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    psa_algorithm_t alg = PSA_ALG_HMAC(PSA_ALG_SHA_256);
    size_t hmac_length = PSA_HASH_LENGTH(alg);
    size_t mac_len = 0;

    setup_hmac_key_attributes(&attributes, alg, PSA_KEY_LIFETIME_VOLATILE);

    uint8_t *hmac = malloc(hmac_length);
    TEST_ASSERT_NOT_NULL(hmac);

    status = psa_import_key(&attributes, key_256, sizeof(key_256), &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    psa_mac_operation_t op = PSA_MAC_OPERATION_INIT;

    test_hmac_multipart_sign(&op, key_id, alg, test_data, sizeof(test_data),
                             hmac, hmac_length, &mac_len);

    test_hmac_multipart_verify(&op, key_id, alg, test_data, sizeof(test_data),
                               expected_hmac_sha256, sizeof(expected_hmac_sha256));

    psa_reset_key_attributes(&attributes);
    psa_mac_abort(&op);
    psa_destroy_key(key_id);
    free(hmac);
}
