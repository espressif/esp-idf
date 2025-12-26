/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_private/periph_ctrl.h"

#include "psa/crypto.h"

#include "unity.h"

static const uint8_t key_256[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
};

TEST_CASE("PSA AES-CTR multipart", "[psa-aes]")
{
    const size_t SZ = 100;
    const size_t iv_SZ = 16;
    const size_t part_size = 8;

    uint8_t *plaintext = malloc(SZ);
    uint8_t *ciphertext = malloc(SZ);
    uint8_t *decryptedtext = malloc(SZ);

    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(decryptedtext);
    uint8_t iv[iv_SZ];

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    /* Import a key */
    psa_key_id_t key_id;
    psa_algorithm_t alg = PSA_ALG_CTR;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, sizeof(key_256) * 8);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_import_key(&attributes, key_256, sizeof(key_256), &key_id));

    psa_reset_key_attributes(&attributes);

    /* Encrypt */
    psa_cipher_operation_t enc_op = PSA_CIPHER_OPERATION_INIT;
    size_t out_len, total_out_len = 0;

    memset(iv, 0x3B, iv_SZ); // Initialize IV with known value
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_encrypt_setup(&enc_op, key_id, alg));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&enc_op, iv, iv_SZ));
    for (size_t offset = 0; offset < SZ; offset += part_size) {
        size_t this_part = SZ - offset < part_size ? SZ - offset : part_size;
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&enc_op, plaintext + offset, this_part,
                                                        ciphertext + offset, this_part, &out_len));
        total_out_len += out_len;
    }

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&enc_op, ciphertext + total_out_len,
                                                    SZ - total_out_len, &out_len));
    total_out_len += out_len;
    TEST_ASSERT_EQUAL_size_t(SZ, total_out_len);

    /* Decrypt */
    psa_cipher_operation_t dec_op = PSA_CIPHER_OPERATION_INIT;
    total_out_len = 0;

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_decrypt_setup(&dec_op, key_id, alg));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&dec_op, iv, iv_SZ));

    for (size_t offset = 0; offset < SZ; offset += part_size) {
        size_t this_part = SZ - offset < part_size ? SZ - offset : part_size;
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&dec_op, ciphertext + offset, this_part,
                                                        decryptedtext + offset, this_part, &out_len));
        total_out_len += out_len;
    }

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&dec_op, decryptedtext + total_out_len,
                                                    SZ - total_out_len, &out_len));
    total_out_len += out_len;
    TEST_ASSERT_EQUAL_size_t(SZ, total_out_len);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    free(plaintext);
    free(ciphertext);
    free(decryptedtext);

    psa_cipher_abort(&enc_op);
    psa_cipher_abort(&dec_op);

    /* Destroy the key */
    psa_destroy_key(key_id);
}

TEST_CASE("PSA AES-ECB multipart", "[psa-aes]")
{
    const size_t SZ = 112;
    const size_t iv_SZ = 16;
    const size_t part_size = 16;

    uint8_t *plaintext = malloc(SZ);
    uint8_t *ciphertext = malloc(SZ);
    uint8_t *decryptedtext = malloc(SZ);

    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(decryptedtext);
    uint8_t iv[iv_SZ];

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    /* Import a key */
    psa_key_id_t key_id;
    psa_algorithm_t alg = PSA_ALG_ECB_NO_PADDING;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, sizeof(key_256) * 8);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_import_key(&attributes, key_256, sizeof(key_256), &key_id));

    psa_reset_key_attributes(&attributes);

    /* Encrypt */
    psa_cipher_operation_t enc_op = PSA_CIPHER_OPERATION_INIT;
    size_t out_len, total_out_len = 0;

    memset(iv, 0x3B, iv_SZ); // Initialize IV with known value
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_encrypt_setup(&enc_op, key_id, alg));
    for (size_t offset = 0; offset < SZ; offset += part_size) {
        size_t this_part = SZ - offset < part_size ? SZ - offset : part_size;
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&enc_op, plaintext + offset, this_part,
                                                        ciphertext + offset, this_part, &out_len));
        total_out_len += out_len;
    }

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&enc_op, ciphertext + total_out_len,
                                                    SZ - total_out_len, &out_len));
    total_out_len += out_len;
    TEST_ASSERT_EQUAL_size_t(SZ, total_out_len);

    /* Decrypt */
    psa_cipher_operation_t dec_op = PSA_CIPHER_OPERATION_INIT;
    total_out_len = 0;

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_decrypt_setup(&dec_op, key_id, alg));

    for (size_t offset = 0; offset < SZ; offset += part_size) {
        size_t this_part = SZ - offset < part_size ? SZ - offset : part_size;
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&dec_op, ciphertext + offset, this_part,
                                                        decryptedtext + offset, this_part, &out_len));
        total_out_len += out_len;
    }

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&dec_op, decryptedtext + total_out_len,
                                                    SZ - total_out_len, &out_len));
    total_out_len += out_len;
    TEST_ASSERT_EQUAL_size_t(SZ, total_out_len);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    free(plaintext);
    free(ciphertext);
    free(decryptedtext);

    psa_cipher_abort(&enc_op);
    psa_cipher_abort(&dec_op);

    /* Destroy the key */
    psa_destroy_key(key_id);
}

TEST_CASE("PSA AES-CBC multipart", "[psa-aes]")
{
    const size_t SZ = 112;  // Multiple of block size (16)
    const size_t iv_SZ = 16;
    const size_t part_size = 16;  // Process one block at a time

    uint8_t *plaintext = malloc(SZ);
    uint8_t *ciphertext = malloc(SZ);
    uint8_t *decryptedtext = malloc(SZ);

    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(decryptedtext);
    uint8_t iv[iv_SZ];

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    /* Import a key */
    psa_key_id_t key_id;
    psa_algorithm_t alg = PSA_ALG_CBC_NO_PADDING;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, sizeof(key_256) * 8);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_import_key(&attributes, key_256, sizeof(key_256), &key_id));

    psa_reset_key_attributes(&attributes);

    /* Encrypt */
    psa_cipher_operation_t enc_op = PSA_CIPHER_OPERATION_INIT;
    size_t out_len, total_out_len = 0;

    memset(iv, 0x3B, iv_SZ); // Initialize IV with known value
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_encrypt_setup(&enc_op, key_id, alg));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&enc_op, iv, iv_SZ));

    for (size_t offset = 0; offset < SZ; offset += part_size) {
        size_t this_part = SZ - offset < part_size ? SZ - offset : part_size;
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&enc_op, plaintext + offset, this_part,
                                                        ciphertext + offset, this_part, &out_len));
        total_out_len += out_len;
    }

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&enc_op, ciphertext + total_out_len,
                                                    SZ - total_out_len, &out_len));
    total_out_len += out_len;
    TEST_ASSERT_EQUAL_size_t(SZ, total_out_len);

    /* Decrypt */
    psa_cipher_operation_t dec_op = PSA_CIPHER_OPERATION_INIT;
    total_out_len = 0;

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_decrypt_setup(&dec_op, key_id, alg));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&dec_op, iv, iv_SZ));

    for (size_t offset = 0; offset < SZ; offset += part_size) {
        size_t this_part = SZ - offset < part_size ? SZ - offset : part_size;
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&dec_op, ciphertext + offset, this_part,
                                                        decryptedtext + offset, this_part, &out_len));
        total_out_len += out_len;
    }

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&dec_op, decryptedtext + total_out_len,
                                                    SZ - total_out_len, &out_len));
    total_out_len += out_len;
    TEST_ASSERT_EQUAL_size_t(SZ, total_out_len);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    free(plaintext);
    free(ciphertext);
    free(decryptedtext);

    psa_cipher_abort(&enc_op);
    psa_cipher_abort(&dec_op);

    /* Destroy the key */
    psa_destroy_key(key_id);
}

TEST_CASE("PSA AES-CBC-PKCS7 multipart", "[psa-aes]")
{
    // Test both aligned and unaligned sizes
    const size_t SZ1 = 112;  // Multiple of block size (16)
    const size_t SZ2 = 123;  // Not a multiple of block size
    const size_t iv_SZ = 16;
    const size_t part_size = 16;

    uint8_t *plaintext1 = malloc(SZ1);
    uint8_t *ciphertext1 = malloc(SZ1 + 16); // Extra block for padding
    uint8_t *decryptedtext1 = malloc(SZ1 + 16); // Extra space for intermediate buffering

    uint8_t *plaintext2 = malloc(SZ2);
    uint8_t *ciphertext2 = malloc(SZ2 + 16); // Extra block for padding
    uint8_t *decryptedtext2 = malloc(SZ2 + 16); // Extra space for intermediate buffering

    uint8_t iv[iv_SZ];

    // Initialize test data
    memset(plaintext1, 0x3A, SZ1);
    memset(plaintext2, 0x3B, SZ2);
    memset(ciphertext1, 0x0, SZ1 + 16);
    memset(ciphertext2, 0x0, SZ2 + 16);
    memset(decryptedtext1, 0x0, SZ1 + 16);
    memset(decryptedtext2, 0x0, SZ2 + 16);

    /* Import a key */
    psa_key_id_t key_id;
    psa_algorithm_t alg = PSA_ALG_CBC_PKCS7;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, sizeof(key_256) * 8);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_import_key(&attributes, key_256, sizeof(key_256), &key_id));

    psa_reset_key_attributes(&attributes);

    /* Test 1: Block-aligned input */
    {
        psa_cipher_operation_t enc_op = PSA_CIPHER_OPERATION_INIT;
        size_t out_len, total_out_len = 0;

        memset(iv, 0x3C, iv_SZ);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_encrypt_setup(&enc_op, key_id, alg));
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&enc_op, iv, iv_SZ));

        // Process all blocks except the last one
        for (size_t offset = 0; offset < SZ1 - part_size; offset += part_size) {
            TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&enc_op, plaintext1 + offset, part_size,
                                                            ciphertext1 + total_out_len, SZ1 + 16 - total_out_len, &out_len));
            total_out_len += out_len;
        }

        // Process the last block separately
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&enc_op, plaintext1 + SZ1 - part_size, part_size,
                                                        ciphertext1 + total_out_len, SZ1 + 16 - total_out_len, &out_len));
        total_out_len += out_len;

        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&enc_op, ciphertext1 + total_out_len,
                                                        SZ1 + 16 - total_out_len, &out_len));  // Space for padding block
        total_out_len += out_len;

        // The output size should be the input size rounded up to the next multiple of 16
        TEST_ASSERT_EQUAL_size_t((SZ1 + 16), total_out_len);  // Should include padding block
        /* Decrypt */
        psa_cipher_operation_t dec_op = PSA_CIPHER_OPERATION_INIT;
        size_t dec_len = 0;

        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_decrypt_setup(&dec_op, key_id, alg));
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&dec_op, iv, iv_SZ));

        for (size_t offset = 0; offset < total_out_len; offset += part_size) {
            size_t this_part = total_out_len - offset < part_size ? total_out_len - offset : part_size;
            TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&dec_op, ciphertext1 + offset, this_part,
                                                            decryptedtext1 + dec_len, SZ1 + 16 - dec_len, &out_len));
            dec_len += out_len;
        }

        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&dec_op, decryptedtext1 + dec_len,
                                                        SZ1 + 16 - dec_len, &out_len));
        dec_len += out_len;

        TEST_ASSERT_EQUAL_size_t(SZ1, dec_len);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext1, decryptedtext1, SZ1);

        psa_cipher_abort(&enc_op);
        psa_cipher_abort(&dec_op);
    }

    /* Test 2: Non-block-aligned input */
    {
        psa_cipher_operation_t enc_op = PSA_CIPHER_OPERATION_INIT;
        size_t out_len, total_out_len = 0;

        memset(iv, 0x3D, iv_SZ);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_encrypt_setup(&enc_op, key_id, alg));
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&enc_op, iv, iv_SZ));

        for (size_t offset = 0; offset < SZ2; offset += part_size) {
            size_t this_part = SZ2 - offset < part_size ? SZ2 - offset : part_size;
            TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&enc_op, plaintext2 + offset, this_part,
                                                            ciphertext2 + total_out_len, SZ2 + 16 - total_out_len, &out_len));
            total_out_len += out_len;
        }

        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&enc_op, ciphertext2 + total_out_len,
                                                        SZ2 + 16 - total_out_len, &out_len));
        total_out_len += out_len;

        /* Decrypt */
        psa_cipher_operation_t dec_op = PSA_CIPHER_OPERATION_INIT;
        size_t dec_len = 0;

        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_decrypt_setup(&dec_op, key_id, alg));
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&dec_op, iv, iv_SZ));

        for (size_t offset = 0; offset < total_out_len; offset += part_size) {
            size_t this_part = total_out_len - offset < part_size ? total_out_len - offset : part_size;
            TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&dec_op, ciphertext2 + offset, this_part,
                                                            decryptedtext2 + dec_len, SZ2 + 16 - dec_len, &out_len));
            dec_len += out_len;
        }

        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&dec_op, decryptedtext2 + dec_len,
                                                        SZ2 + 16 - dec_len, &out_len));
        dec_len += out_len;

        TEST_ASSERT_EQUAL_size_t(SZ2, dec_len);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext2, decryptedtext2, SZ2);

        psa_cipher_abort(&enc_op);
        psa_cipher_abort(&dec_op);
    }

    /* Cleanup */
    free(plaintext1);
    free(ciphertext1);
    free(decryptedtext1);
    free(plaintext2);
    free(ciphertext2);
    free(decryptedtext2);

    psa_destroy_key(key_id);
}

TEST_CASE("PSA AES-CFB multipart", "[psa-aes]")
{
    const size_t SZ = 100;
    const size_t iv_SZ = 16;
    const size_t part_size = 8;

    uint8_t *plaintext = malloc(SZ);
    uint8_t *ciphertext = malloc(SZ);
    uint8_t *decryptedtext = malloc(SZ);
    uint8_t iv[iv_SZ];

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    /* Import a key */
    psa_key_id_t key_id;
    psa_algorithm_t alg = PSA_ALG_CFB;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, sizeof(key_256) * 8);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_import_key(&attributes, key_256, sizeof(key_256), &key_id));

    psa_reset_key_attributes(&attributes);

    /* Encrypt */
    psa_cipher_operation_t enc_op = PSA_CIPHER_OPERATION_INIT;
    size_t out_len, total_out_len = 0;

    memset(iv, 0x3B, iv_SZ); // Initialize IV with known value
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_encrypt_setup(&enc_op, key_id, alg));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&enc_op, iv, iv_SZ));
    for (size_t offset = 0; offset < SZ; offset += part_size) {
        size_t this_part = SZ - offset < part_size ? SZ - offset : part_size;
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&enc_op, plaintext + offset, this_part,
                                                        ciphertext + offset, this_part, &out_len));
        total_out_len += out_len;
    }

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&enc_op, ciphertext + total_out_len,
                                                    SZ - total_out_len, &out_len));
    total_out_len += out_len;
    TEST_ASSERT_EQUAL_size_t(SZ, total_out_len);

    /* Decrypt */
    psa_cipher_operation_t dec_op = PSA_CIPHER_OPERATION_INIT;
    total_out_len = 0;

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_decrypt_setup(&dec_op, key_id, alg));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&dec_op, iv, iv_SZ));

    for (size_t offset = 0; offset < SZ; offset += part_size) {
        size_t this_part = SZ - offset < part_size ? SZ - offset : part_size;
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&dec_op, ciphertext + offset, this_part,
                                                        decryptedtext + offset, this_part, &out_len));
        total_out_len += out_len;
    }

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&dec_op, decryptedtext + total_out_len,
                                                    SZ - total_out_len, &out_len));
    total_out_len += out_len;
    TEST_ASSERT_EQUAL_size_t(SZ, total_out_len);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    free(plaintext);
    free(ciphertext);
    free(decryptedtext);

    psa_cipher_abort(&enc_op);
    psa_cipher_abort(&dec_op);

    /* Destroy the key */
    psa_destroy_key(key_id);
}

TEST_CASE("PSA AES-OFB multipart", "[psa-aes]")
{
    const size_t SZ = 100;
    const size_t iv_SZ = 16;
    const size_t part_size = 8;

    uint8_t *plaintext = malloc(SZ);
    uint8_t *ciphertext = malloc(SZ);
    uint8_t *decryptedtext = malloc(SZ);
    uint8_t iv[iv_SZ];

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    /* Import a key */
    psa_key_id_t key_id;
    psa_algorithm_t alg = PSA_ALG_OFB;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, sizeof(key_256) * 8);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_import_key(&attributes, key_256, sizeof(key_256), &key_id));

    psa_reset_key_attributes(&attributes);

    /* Encrypt */
    psa_cipher_operation_t enc_op = PSA_CIPHER_OPERATION_INIT;
    size_t out_len, total_out_len = 0;

    memset(iv, 0x3B, iv_SZ); // Initialize IV with known value
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_encrypt_setup(&enc_op, key_id, alg));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&enc_op, iv, iv_SZ));
    for (size_t offset = 0; offset < SZ; offset += part_size) {
        size_t this_part = SZ - offset < part_size ? SZ - offset : part_size;
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&enc_op, plaintext + offset, this_part,
                                                        ciphertext + offset, this_part, &out_len));
        total_out_len += out_len;
    }

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&enc_op, ciphertext + total_out_len,
                                                    SZ - total_out_len, &out_len));
    total_out_len += out_len;
    TEST_ASSERT_EQUAL_size_t(SZ, total_out_len);

    /* Decrypt */
    psa_cipher_operation_t dec_op = PSA_CIPHER_OPERATION_INIT;
    total_out_len = 0;

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_decrypt_setup(&dec_op, key_id, alg));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&dec_op, iv, iv_SZ));

    for (size_t offset = 0; offset < SZ; offset += part_size) {
        size_t this_part = SZ - offset < part_size ? SZ - offset : part_size;
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&dec_op, ciphertext + offset, this_part,
                                                        decryptedtext + offset, this_part, &out_len));
        total_out_len += out_len;
    }

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&dec_op, decryptedtext + total_out_len,
                                                    SZ - total_out_len, &out_len));
    total_out_len += out_len;
    TEST_ASSERT_EQUAL_size_t(SZ, total_out_len);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    free(plaintext);
    free(ciphertext);
    free(decryptedtext);

    psa_cipher_abort(&enc_op);
    psa_cipher_abort(&dec_op);

    /* Destroy the key */
    psa_destroy_key(key_id);
}

TEST_CASE("PSA AES-CTR streaming chunk invariance", "[psa-aes]")
{
    const size_t SZ = 100;
    const size_t iv_SZ = 16;

    // Vectors match legacy mbedtls CTR stream test
    const uint8_t expected_cipher[] = {
        0xc5, 0x78, 0xa7, 0xb4, 0xf3, 0xb9, 0xcb, 0x8b,
        0x09, 0xe0, 0xd6, 0x89, 0x14, 0x6a, 0x19, 0x09,
        0xde, 0xaf, 0x37, 0x19, 0x32, 0x4d, 0xca, 0xf6,
        0xff, 0x6e, 0xd2, 0x5d, 0x87, 0x51, 0xaa, 0x8c,
        0x1c, 0xe3, 0x3b, 0xbb, 0x18, 0xf5, 0xa0, 0x1b,
        0xdc, 0x29, 0x52, 0x63, 0xf6, 0x5d, 0x49, 0x85,
        0x29, 0xf1, 0xf0, 0x69, 0x8f, 0xa6, 0x9f, 0x38,
        0x5c, 0xdd, 0x26, 0xf8, 0x9d, 0x40, 0xa1, 0xff,
        0x52, 0x46, 0xe1, 0x72, 0x70, 0x39, 0x73, 0xff,
        0xd0, 0x5e, 0xe5, 0x3f, 0xc5, 0xed, 0x5c, 0x18,
        0xa7, 0x84, 0xd8, 0xdf, 0x9d, 0xb5, 0x06, 0xb1,
        0xa7, 0xcf, 0x2e, 0x7a, 0x51, 0xfc, 0x44, 0xc5,
        0xb9, 0x5f, 0x22, 0x47,
    };

    uint8_t key[16];
    uint8_t iv[iv_SZ];
    uint8_t *plaintext = malloc(SZ);
    uint8_t *ciphertext = malloc(SZ);
    uint8_t *decryptedtext = malloc(SZ);

    memset(key, 0x44, sizeof(key));
    memset(iv, 0xEE, iv_SZ);
    memset(plaintext, 0xAA, SZ);

    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CTR);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, sizeof(key) * 8);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_import_key(&attributes, key, sizeof(key), &key_id));
    psa_reset_key_attributes(&attributes);

    for (size_t chunk = 1; chunk < SZ; chunk++) {
        psa_cipher_operation_t enc_op = PSA_CIPHER_OPERATION_INIT;
        size_t out_len = 0, total_out = 0;

        memset(iv, 0xEE, iv_SZ);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_encrypt_setup(&enc_op, key_id, PSA_ALG_CTR));
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&enc_op, iv, iv_SZ));

        for (size_t offset = 0; offset < SZ; offset += chunk) {
            size_t this_part = SZ - offset < chunk ? SZ - offset : chunk;
            TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&enc_op, plaintext + offset, this_part,
                                                            ciphertext + offset, this_part, &out_len));
            total_out += out_len;
        }

        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&enc_op, ciphertext + total_out,
                                                        SZ - total_out, &out_len));
        total_out += out_len;
        TEST_ASSERT_EQUAL_size_t(SZ, total_out);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher, ciphertext, SZ);

        psa_cipher_abort(&enc_op);

        psa_cipher_operation_t dec_op = PSA_CIPHER_OPERATION_INIT;
        total_out = 0;
        memset(iv, 0xEE, iv_SZ);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_decrypt_setup(&dec_op, key_id, PSA_ALG_CTR));
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&dec_op, iv, iv_SZ));

        for (size_t offset = 0; offset < SZ; offset += chunk) {
            size_t this_part = SZ - offset < chunk ? SZ - offset : chunk;
            TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&dec_op, ciphertext + offset, this_part,
                                                            decryptedtext + offset, this_part, &out_len));
            total_out += out_len;
        }

        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&dec_op, decryptedtext + total_out,
                                                        SZ - total_out, &out_len));
        total_out += out_len;
        TEST_ASSERT_EQUAL_size_t(SZ, total_out);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

        psa_cipher_abort(&dec_op);
    }

    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
    psa_destroy_key(key_id);
}

TEST_CASE("PSA AES-OFB streaming chunk invariance", "[psa-aes]")
{
    const size_t SZ = 100;
    const size_t iv_SZ = 16;

    // Vectors match legacy mbedtls OFB stream test
    const uint8_t expected_cipher[] = {
        0xc5, 0x78, 0xa7, 0xb4, 0xf3, 0xb9, 0xcb, 0x8b,
        0x09, 0xe0, 0xd6, 0x89, 0x14, 0x6a, 0x19, 0x09,
        0x0a, 0x33, 0x8b, 0xab, 0x82, 0xcb, 0x20, 0x8f,
        0x74, 0x2a, 0x6c, 0xb3, 0xc6, 0xe8, 0x18, 0x89,
        0x09, 0xb6, 0xaf, 0x20, 0xcd, 0xea, 0x74, 0x14,
        0x48, 0x61, 0xe8, 0x4d, 0x50, 0x12, 0x9f, 0x5e,
        0xb8, 0x10, 0x53, 0x3b, 0x74, 0xd9, 0xd0, 0x95,
        0x13, 0xdc, 0x14, 0xcf, 0x0c, 0xa1, 0x90, 0xfd,
        0xa2, 0x58, 0x12, 0xb2, 0x00, 0x2c, 0x5b, 0x7a,
        0x2a, 0x76, 0x80, 0x20, 0x82, 0x39, 0xa2, 0x21,
        0xf8, 0x7a, 0xec, 0xae, 0x82, 0x6a, 0x5c, 0xd3,
        0x04, 0xd9, 0xbd, 0xe4, 0x53, 0xc9, 0xdf, 0x67,
        0xaa, 0x5c, 0xaf, 0xa6,
    };

    uint8_t key[16];
    uint8_t iv[iv_SZ];
    uint8_t *plaintext = malloc(SZ);
    uint8_t *ciphertext = malloc(SZ);
    uint8_t *decryptedtext = malloc(SZ);

    memset(key, 0x44, sizeof(key));
    memset(iv, 0xEE, iv_SZ);
    memset(plaintext, 0xAA, SZ);

    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_OFB);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, sizeof(key) * 8);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_import_key(&attributes, key, sizeof(key), &key_id));
    psa_reset_key_attributes(&attributes);

    for (size_t chunk = 1; chunk < SZ; chunk++) {
        psa_cipher_operation_t enc_op = PSA_CIPHER_OPERATION_INIT;
        size_t out_len = 0, total_out = 0;

        memset(iv, 0xEE, iv_SZ);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_encrypt_setup(&enc_op, key_id, PSA_ALG_OFB));
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&enc_op, iv, iv_SZ));

        for (size_t offset = 0; offset < SZ; offset += chunk) {
            size_t this_part = SZ - offset < chunk ? SZ - offset : chunk;
            TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&enc_op, plaintext + offset, this_part,
                                                            ciphertext + offset, this_part, &out_len));
            total_out += out_len;
        }

        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&enc_op, ciphertext + total_out,
                                                        SZ - total_out, &out_len));
        total_out += out_len;
        TEST_ASSERT_EQUAL_size_t(SZ, total_out);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher, ciphertext, SZ);

        psa_cipher_abort(&enc_op);

        psa_cipher_operation_t dec_op = PSA_CIPHER_OPERATION_INIT;
        total_out = 0;
        memset(iv, 0xEE, iv_SZ);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_decrypt_setup(&dec_op, key_id, PSA_ALG_OFB));
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&dec_op, iv, iv_SZ));

        for (size_t offset = 0; offset < SZ; offset += chunk) {
            size_t this_part = SZ - offset < chunk ? SZ - offset : chunk;
            TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&dec_op, ciphertext + offset, this_part,
                                                            decryptedtext + offset, this_part, &out_len));
            total_out += out_len;
        }

        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&dec_op, decryptedtext + total_out,
                                                        SZ - total_out, &out_len));
        total_out += out_len;
        TEST_ASSERT_EQUAL_size_t(SZ, total_out);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

        psa_cipher_abort(&dec_op);
    }

    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
    psa_destroy_key(key_id);
}

TEST_CASE("PSA AES-CFB-128", "[psa-aes]")
{
    const size_t SZ = 1000;
    const size_t iv_SZ = 16;
    const uint8_t expected_cipher_end[] = {
        0xf3, 0x64, 0x20, 0xa1, 0x70, 0x2a, 0xd9, 0x3f,
        0xb7, 0x48, 0x8c, 0x2c, 0x1f, 0x65, 0x53, 0xc2,
        0xac, 0xfd, 0x82, 0xe5, 0x31, 0x24, 0x1f, 0x30,
        0xaf, 0xcc, 0x8d, 0xb3, 0xf3, 0x63, 0xe1, 0xa0,
    };

    const uint8_t iv_seed[] = {
        0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09,
        0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,
    };
    uint8_t iv[iv_SZ];
    uint8_t *plaintext = malloc(SZ);
    uint8_t *ciphertext = malloc(SZ);
    uint8_t *decryptedtext = malloc(SZ);

    memcpy(iv, iv_seed, iv_SZ);
    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CFB);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, sizeof(key_256) * 8);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_import_key(&attributes, key_256, sizeof(key_256), &key_id));
    psa_reset_key_attributes(&attributes);

    psa_cipher_operation_t enc_op = PSA_CIPHER_OPERATION_INIT;
    size_t out_len = 0, total_out = 0;

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_encrypt_setup(&enc_op, key_id, PSA_ALG_CFB));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&enc_op, iv, iv_SZ));

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&enc_op, plaintext, SZ, ciphertext, SZ, &out_len));
    total_out += out_len;
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&enc_op, ciphertext + total_out, SZ - total_out, &out_len));
    total_out += out_len;
    TEST_ASSERT_EQUAL_size_t(SZ, total_out);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_end, ciphertext + SZ - sizeof(expected_cipher_end), sizeof(expected_cipher_end));

    psa_cipher_operation_t dec_op = PSA_CIPHER_OPERATION_INIT;
    total_out = 0;
    memcpy(iv, iv_seed, iv_SZ);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_decrypt_setup(&dec_op, key_id, PSA_ALG_CFB));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&dec_op, iv, iv_SZ));

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&dec_op, ciphertext, SZ, decryptedtext, SZ, &out_len));
    total_out += out_len;
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&dec_op, decryptedtext + total_out, SZ - total_out, &out_len));
    total_out += out_len;
    TEST_ASSERT_EQUAL_size_t(SZ, total_out);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    psa_cipher_abort(&enc_op);
    psa_cipher_abort(&dec_op);
    psa_destroy_key(key_id);

    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}


TEST_CASE("PSA AES-CBC one-shot", "[psa-aes]")
{
    const size_t SZ = 1600;
    const size_t iv_SZ = 16;

    // allocate internal memory
    uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    uint8_t *ciphertext = heap_caps_malloc(SZ + iv_SZ, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    /* Import a key */
    psa_key_id_t key_id;
    psa_algorithm_t alg = PSA_ALG_CBC_NO_PADDING;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, sizeof(key_256) * 8);
    TEST_ASSERT_EQUAL(psa_import_key(&attributes, key_256, sizeof(key_256), &key_id), PSA_SUCCESS);

    psa_reset_key_attributes(&attributes);

    size_t ciphertext_len = 0;
    /* Encrypt the plaintext */
    TEST_ASSERT_EQUAL(psa_cipher_encrypt(key_id, alg, plaintext, SZ, ciphertext, SZ + iv_SZ, &ciphertext_len), PSA_SUCCESS);

    TEST_ASSERT_EQUAL_size_t(ciphertext_len, SZ + iv_SZ);

    size_t decryptedtext_len = 0;
    /* Decrypt the ciphertext */
    TEST_ASSERT_EQUAL(psa_cipher_decrypt(key_id, alg, ciphertext, SZ + iv_SZ, decryptedtext, SZ, &decryptedtext_len), PSA_SUCCESS);

    TEST_ASSERT_EQUAL_size_t(decryptedtext_len, SZ);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    free(plaintext);
    free(ciphertext);
    free(decryptedtext);

    /* Destroy the key */
    psa_destroy_key(key_id);
}

static const uint8_t key_192[24] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
};

TEST_CASE("PSA AES-CBC-192", "[psa-aes]")
{
    const size_t SZ = 1008;
    const size_t iv_SZ = 16;

    const uint8_t expected_cipher_end[] = {
        0x57, 0x6a, 0x75, 0xb4, 0x5d, 0xbc, 0x96, 0xf8,
        0xa4, 0xb7, 0xb6, 0x0c, 0x6b, 0xa5, 0x1e, 0x02,
    };

    const uint8_t iv_seed[] = {
        0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09,
        0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,
    };

    uint8_t iv[iv_SZ];
    uint8_t *plaintext = malloc(SZ);
    uint8_t *ciphertext = malloc(SZ);
    uint8_t *decryptedtext = malloc(SZ);

    memcpy(iv, iv_seed, iv_SZ);
    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CBC_NO_PADDING);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 192);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_import_key(&attributes, key_192, sizeof(key_192), &key_id));
    psa_reset_key_attributes(&attributes);

    psa_cipher_operation_t enc_op = PSA_CIPHER_OPERATION_INIT;
    size_t out_len = 0, total_out = 0;

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_encrypt_setup(&enc_op, key_id, PSA_ALG_CBC_NO_PADDING));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&enc_op, iv, iv_SZ));

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&enc_op, plaintext, SZ, ciphertext, SZ, &out_len));
    total_out += out_len;
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&enc_op, ciphertext + total_out, SZ - total_out, &out_len));
    total_out += out_len;
    TEST_ASSERT_EQUAL_size_t(SZ, total_out);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_end, ciphertext + SZ - sizeof(expected_cipher_end), sizeof(expected_cipher_end));

    psa_cipher_operation_t dec_op = PSA_CIPHER_OPERATION_INIT;
    total_out = 0;
    memcpy(iv, iv_seed, iv_SZ);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_decrypt_setup(&dec_op, key_id, PSA_ALG_CBC_NO_PADDING));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&dec_op, iv, iv_SZ));

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&dec_op, ciphertext, SZ, decryptedtext, SZ, &out_len));
    total_out += out_len;
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&dec_op, decryptedtext + total_out, SZ - total_out, &out_len));
    total_out += out_len;
    TEST_ASSERT_EQUAL_size_t(SZ, total_out);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    psa_cipher_abort(&enc_op);
    psa_cipher_abort(&dec_op);
    psa_destroy_key(key_id);

    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}
