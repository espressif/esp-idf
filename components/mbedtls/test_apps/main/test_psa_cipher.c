/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include <stdio.h>
#include "psa/crypto.h"
#include "unity.h"
#include "esp_log.h"
#include "sdkconfig.h"

#if CONFIG_MBEDTLS_ARIA_C
/*
    ARIA-256-ECB Encrypt - RFC 5794 test vector
    Key:   000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f
    Input: 00112233445566778899aabbccddeeff
    Output:f92bd7c79fb72e2f2b8f80c1972d24fc
*/

static const uint8_t aria_256_key[32] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
    0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f
};

static const uint8_t aria_ecb_input[16] = {
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
    0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff
};

static const uint8_t aria_ecb_expected_output[16] = {
    0xf9,0x2b,0xd7,0xc7,0x9f,0xb7,0x2e,0x2f,
    0x2b,0x8f,0x80,0xc1,0x97,0x2d,0x24,0xfc
};

TEST_CASE("PSA ARIA-256-ECB encrypt test vector", "[psa][psa_cipher][aria]")
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_attributes_t key_attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    uint8_t output[16] = {0};
    size_t output_len = 0;

    status = psa_crypto_init();
    TEST_ASSERT_EQUAL_HEX32(PSA_SUCCESS, status);

    psa_set_key_type(&key_attr, PSA_KEY_TYPE_ARIA);
    psa_set_key_bits(&key_attr, 256);
    psa_set_key_usage_flags(&key_attr, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&key_attr, PSA_ALG_ECB_NO_PADDING);

    status = psa_import_key(&key_attr, aria_256_key, sizeof(aria_256_key), &key_id);
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(PSA_SUCCESS, status, "psa_import_key failed");

    status = psa_cipher_encrypt(
        key_id,
        PSA_ALG_ECB_NO_PADDING,
        aria_ecb_input, sizeof(aria_ecb_input),
        output, sizeof(output), &output_len
    );
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(PSA_SUCCESS, status, "psa_cipher_encrypt failed");
    TEST_ASSERT_EQUAL_UINT_MESSAGE(sizeof(aria_ecb_expected_output), output_len, "Output length mismatch");
    TEST_ASSERT_EQUAL_UINT8_ARRAY(aria_ecb_expected_output, output, sizeof(aria_ecb_expected_output));
    psa_destroy_key(key_id);

    // Test decryption: decrypt the previously encrypted ciphertext and compare with original plaintext
    uint8_t decrypted[16] = {0};
    size_t decrypted_len = 0;

    // To decrypt, we need a key with PSA_KEY_USAGE_DECRYPT usage
    psa_key_attributes_t dec_key_attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&dec_key_attr, PSA_KEY_TYPE_ARIA);
    psa_set_key_bits(&dec_key_attr, 256);
    psa_set_key_usage_flags(&dec_key_attr, PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&dec_key_attr, PSA_ALG_ECB_NO_PADDING);

    psa_key_id_t dec_key_id = 0;
    status = psa_import_key(&dec_key_attr, aria_256_key, sizeof(aria_256_key), &dec_key_id);
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(PSA_SUCCESS, status, "psa_import_key (decrypt key) failed");

    status = psa_cipher_decrypt(
        dec_key_id,
        PSA_ALG_ECB_NO_PADDING,
        output, sizeof(output),
        decrypted, sizeof(decrypted), &decrypted_len
    );
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(PSA_SUCCESS, status, "psa_cipher_decrypt failed");
    TEST_ASSERT_EQUAL_UINT_MESSAGE(sizeof(aria_ecb_input), decrypted_len, "Decrypted length mismatch");
    TEST_ASSERT_EQUAL_UINT8_ARRAY(aria_ecb_input, decrypted, sizeof(aria_ecb_input));

    psa_destroy_key(dec_key_id);
}
#endif /* CONFIG_MBEDTLS_ARIA_C */
