/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* PSA HMAC test
*/

#include "psa/crypto.h"
#include "unity.h"
static const uint8_t key_128[] = {
    0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
    0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
};

static const uint8_t test_data[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51
};

static const uint8_t expected_hmac_128[] = {
    0x00, 0x7a, 0x5a, 0xd6, 0x54, 0x96, 0x5b, 0xcd,
    0x30, 0xc1, 0x60, 0x62, 0xec, 0xac, 0x75, 0xfb,
    0x87, 0x71, 0x0e, 0x13
};

TEST_CASE("PSA HMAC SHA-1 test", "[psa_hmac]")
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;

    // Initialize PSA Crypto
    status = PSA_SUCCESS;
    // TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&attributes, PSA_ALG_HMAC(PSA_ALG_SHA_1));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_HMAC);
    psa_set_key_bits(&attributes, 128);

    uint8_t *hmac = malloc(PSA_HASH_LENGTH(PSA_ALG_SHA_1));
    TEST_ASSERT_NOT_NULL(hmac);

    status = psa_import_key(&attributes, key_128, sizeof(key_128), &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    size_t mac_length = 0;
    status = psa_mac_compute(key_id, PSA_ALG_HMAC(PSA_ALG_SHA_1),
                             test_data, sizeof(test_data),
                             hmac, PSA_HASH_LENGTH(PSA_ALG_SHA_1), &mac_length);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_mac_verify(key_id, PSA_ALG_HMAC(PSA_ALG_SHA_1),
                             test_data, sizeof(test_data),
                             expected_hmac_128, sizeof(expected_hmac_128));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    psa_destroy_key(key_id);
    psa_reset_key_attributes(&attributes);
    free(hmac);
}
