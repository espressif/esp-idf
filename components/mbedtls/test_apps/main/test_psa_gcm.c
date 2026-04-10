/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "esp_log.h"

#include "psa/crypto.h"
#include "unity.h"
#include "sdkconfig.h"

#ifdef CONFIG_MBEDTLS_ARIA_C
static const uint8_t key_256[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
};

TEST_CASE("PSA ARIA-GCM multipart", "[psa-gcm][aria]")
{
    const size_t SZ = 100;
    const size_t iv_SZ = 12;  // GCM typically uses 12 bytes IV
    const size_t tag_SZ = 16; // GCM tag size
    const size_t aad_SZ = 16; // Size of Additional Authenticated Data
    const size_t part_size = 8;

    // Allocate memory with proper alignment
    uint8_t *plaintext = malloc(SZ);
    uint8_t *ciphertext = malloc(SZ + tag_SZ);
    uint8_t *decryptedtext = malloc(SZ);
    uint8_t *iv = malloc(iv_SZ);
    uint8_t *aad = malloc(aad_SZ);

    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(decryptedtext);
    TEST_ASSERT_NOT_NULL(iv);
    TEST_ASSERT_NOT_NULL(aad);

    // Initialize test data
    memset(plaintext, 0x3A, SZ);
    memset(ciphertext, 0, SZ + tag_SZ);
    memset(decryptedtext, 0x0, SZ);
    memset(iv, 0x3B, iv_SZ);
    memset(aad, 0x3C, aad_SZ);

    /* Import a key */
    psa_key_id_t key_id;
    psa_algorithm_t alg = PSA_ALG_GCM;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_ARIA);
    psa_set_key_bits(&attributes, sizeof(key_256) * 8);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_import_key(&attributes, key_256, sizeof(key_256), &key_id));
    psa_reset_key_attributes(&attributes);

    /* Encrypt */
    psa_aead_operation_t enc_op = PSA_AEAD_OPERATION_INIT;
    size_t out_len, total_out_len = 0;
    size_t tag_length = 0;

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_aead_encrypt_setup(&enc_op, key_id, alg));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_aead_set_lengths(&enc_op, aad_SZ, SZ));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_aead_set_nonce(&enc_op, iv, iv_SZ));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_aead_update_ad(&enc_op, aad, aad_SZ));

    // Process the plaintext in parts
    for (size_t offset = 0; offset < SZ; offset += part_size) {
        size_t this_part = SZ - offset < part_size ? SZ - offset : part_size;
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_aead_update(&enc_op, plaintext + offset, this_part,
                                                      ciphertext + offset, this_part, &out_len));
        total_out_len += out_len;
    }

    // Finish encryption and get the tag
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_aead_finish(&enc_op,
                                                  ciphertext + total_out_len,
                                                  SZ + tag_SZ - total_out_len,
                                                  &out_len,
                                                  ciphertext + SZ,
                                                  tag_SZ,
                                                  &tag_length));
    total_out_len += out_len;

    /* Decrypt */
    psa_aead_operation_t dec_op = PSA_AEAD_OPERATION_INIT;
    total_out_len = 0;

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_aead_decrypt_setup(&dec_op, key_id, alg));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_aead_set_lengths(&dec_op, aad_SZ, SZ));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_aead_set_nonce(&dec_op, iv, iv_SZ));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_aead_update_ad(&dec_op, aad, aad_SZ));

    // Process the ciphertext in parts
    for (size_t offset = 0; offset < SZ; offset += part_size) {
        size_t this_part = SZ - offset < part_size ? SZ - offset : part_size;
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_aead_update(&dec_op, ciphertext + offset, this_part,
                                                      decryptedtext + offset, this_part, &out_len));
        total_out_len += out_len;
    }

    // Verify the tag and finish decryption
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_aead_verify(&dec_op,
                                                  decryptedtext + total_out_len,
                                                  SZ - total_out_len,
                                                  &out_len,
                                                  ciphertext + SZ,
                                                  tag_SZ));
    total_out_len += out_len;

    // Verify the decrypted data matches the original plaintext
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    /* Cleanup */
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
    free(iv);
    free(aad);

    psa_aead_abort(&enc_op);
    psa_aead_abort(&dec_op);

    /* Destroy the key */
    psa_destroy_key(key_id);
}

TEST_CASE("PSA ARIA-GCM one-shot", "[psa-gcm][aria]")
{
    // TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_crypto_init());

    const size_t SZ = 100;
    const size_t iv_SZ = 12;  // GCM typically uses 12 bytes IV
    const size_t tag_SZ = 16; // GCM tag size
    const size_t aad_SZ = 16; // Size of Additional Authenticated Data

    // Allocate memory with proper alignment
    uint8_t *plaintext = malloc(SZ);
    uint8_t *ciphertext = malloc(SZ + tag_SZ);
    uint8_t *decryptedtext = malloc(SZ);
    uint8_t *iv = malloc(iv_SZ);
    uint8_t *aad = malloc(aad_SZ);

    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(decryptedtext);
    TEST_ASSERT_NOT_NULL(iv);
    TEST_ASSERT_NOT_NULL(aad);

    // Initialize test data
    memset(plaintext, 0x3A, SZ);
    memset(ciphertext, 0, SZ + tag_SZ);
    memset(decryptedtext, 0x0, SZ);
    memset(iv, 0x3B, iv_SZ);
    memset(aad, 0x3C, aad_SZ);

    /* Import a key */
    psa_key_id_t key_id;
    psa_algorithm_t alg = PSA_ALG_GCM;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_ARIA);
    psa_set_key_bits(&attributes, sizeof(key_256) * 8);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_import_key(&attributes, key_256, sizeof(key_256), &key_id));
    psa_reset_key_attributes(&attributes);

    size_t output_length;

    /* One-shot encrypt */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_aead_encrypt(key_id, alg,
                                                   iv, iv_SZ,
                                                   aad, aad_SZ,
                                                   plaintext, SZ,
                                                   ciphertext, SZ + tag_SZ,
                                                   &output_length));

    TEST_ASSERT_EQUAL_size_t(SZ + tag_SZ, output_length);

    /* One-shot decrypt */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_aead_decrypt(key_id, alg,
                                                   iv, iv_SZ,
                                                   aad, aad_SZ,
                                                   ciphertext, SZ + tag_SZ,
                                                   decryptedtext, SZ,
                                                   &output_length));

    TEST_ASSERT_EQUAL_size_t(SZ, output_length);

    // Verify the decrypted data matches the original plaintext
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    /* Cleanup */
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
    free(iv);
    free(aad);

    /* Destroy the key */
    psa_destroy_key(key_id);
}
#endif /* CONFIG_MBEDTLS_ARIA_C */
