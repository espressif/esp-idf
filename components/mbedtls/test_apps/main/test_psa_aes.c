/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_private/periph_ctrl.h"

#include "mbedtls/aes.h"
#include "mbedtls/cipher.h"

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
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_crypto_init());

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
    mbedtls_psa_crypto_free();
}

TEST_CASE("PSA AES-ECB multipart", "[psa-aes]")
{
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_crypto_init());

    const size_t SZ = 112;
    const size_t iv_SZ = 16;
    const size_t part_size = 16;

    uint8_t *plaintext = malloc(SZ);
    uint8_t *ciphertext = malloc(SZ);
    uint8_t *decryptedtext = malloc(SZ);
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
    mbedtls_psa_crypto_free();
}

TEST_CASE("PSA AES-CBC multipart", "[psa-aes]")
{
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_crypto_init());

    const size_t SZ = 112;  // Multiple of block size (16)
    const size_t iv_SZ = 16;
    const size_t part_size = 16;  // Process one block at a time

    uint8_t *plaintext = malloc(SZ);
    uint8_t *ciphertext = malloc(SZ);
    uint8_t *decryptedtext = malloc(SZ);
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
    mbedtls_psa_crypto_free();
}

#if 0
TEST_CASE("PSA AES-CBC-PKCS7 multipart", "[psa-aes]")
{
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_crypto_init());

    // Test both aligned and unaligned sizes
    const size_t SZ1 = 112;  // Multiple of block size (16)
    const size_t SZ2 = 123;  // Not a multiple of block size
    const size_t iv_SZ = 16;
    const size_t part_size = 16;

    uint8_t *plaintext1 = malloc(SZ1);
    uint8_t *ciphertext1 = malloc(SZ1 + 16); // Extra block for padding
    uint8_t *decryptedtext1 = malloc(SZ1);

    uint8_t *plaintext2 = malloc(SZ2);
    uint8_t *ciphertext2 = malloc(SZ2 + 16); // Extra block for padding
    uint8_t *decryptedtext2 = malloc(SZ2);

    uint8_t iv[iv_SZ];

    // Initialize test data
    memset(plaintext1, 0x3A, SZ1);
    memset(plaintext2, 0x3B, SZ2);
    memset(decryptedtext1, 0x0, SZ1);
    memset(decryptedtext2, 0x0, SZ2);

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
                                                            ciphertext1 + total_out_len, part_size, &out_len));
            total_out_len += out_len;
        }

        // Process the last block separately
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&enc_op, plaintext1 + SZ1 - part_size, part_size,
                                                        ciphertext1 + total_out_len, part_size + 16, &out_len));
        total_out_len += out_len;

        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&enc_op, ciphertext1 + total_out_len,
                                                        16, &out_len));  // Space for padding block
        total_out_len += out_len;

        // The output size should be the input size rounded up to the next multiple of 16
        TEST_ASSERT_EQUAL_size_t((SZ1 + 16), total_out_len);  // Should include padding block

        ESP_LOGI("TAG", "Decryption");
        /* Decrypt */
        psa_cipher_operation_t dec_op = PSA_CIPHER_OPERATION_INIT;
        size_t dec_len = 0;

        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_decrypt_setup(&dec_op, key_id, alg));
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&dec_op, iv, iv_SZ));

        for (size_t offset = 0; offset < total_out_len; offset += part_size) {
            size_t this_part = total_out_len - offset < part_size ? total_out_len - offset : part_size;
            TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&dec_op, ciphertext1 + offset, this_part,
                                                            decryptedtext1 + dec_len, SZ1 - dec_len, &out_len));
            dec_len += out_len;
        }

        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&dec_op, decryptedtext1 + dec_len,
                                                        SZ1 - dec_len, &out_len));
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
                                                            decryptedtext2 + dec_len, SZ2 - dec_len, &out_len));
            dec_len += out_len;
        }

        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&dec_op, decryptedtext2 + dec_len,
                                                        SZ2 - dec_len, &out_len));
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
    mbedtls_psa_crypto_free();
}
#endif

TEST_CASE("PSA AES-CFB multipart", "[psa-aes]")
{
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_crypto_init());

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
    mbedtls_psa_crypto_free();
}

TEST_CASE("PSA AES-OFB multipart", "[psa-aes]")
{
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_crypto_init());

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
    mbedtls_psa_crypto_free();
}


TEST_CASE("PSA AES-CBC one-shot", "[psa-aes]")
{
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_crypto_init());

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

    mbedtls_psa_crypto_free();
}
