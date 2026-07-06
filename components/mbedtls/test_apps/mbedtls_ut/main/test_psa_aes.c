/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>
#include <stdint.h>
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
#include "test_aes_params.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_macros.h"
#include "freertos/semphr.h"
#include "esp_memory_utils.h"
#include "soc/lldesc.h"

#define INTERNAL_DMA_CAPS (MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL)
#define PSRAM_DMA_CAPS (MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM)

#define TEST_AES_CBC_DMA_MODE_LEN 1600
#define TEST_AES_CTR_DMA_MODE_LEN 1000
#define TEST_AES_OFB_DMA_MODE_LEN 1000
#define TEST_AES_CFB8_DMA_MODE_LEN 1000
#define TEST_AES_CFB128_DMA_MODE_LEN 1000
#define TEST_AES_CTR_STREAM_DMA_MODE_LEN 1000
#define TEST_AES_OFB_STREAM_DMA_MODE_LEN 1000
#define TEST_AES_CFB8_STREAM_DMA_MODE_LEN 1000
#define TEST_AES_CFB128_STREAM_DMA_MODE_LEN 1000

static const uint8_t key_256[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
};

static const uint8_t iv[] = {
    0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09,
    0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,
};

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


static void aes_cbc_test(unsigned int SZ)
{
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    uint8_t nonce[16];
    psa_status_t status;
    size_t output_len, total_output_len;


    memcpy(nonce, iv, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *plaintext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CBC_NO_PADDING);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 256);

    // Import key
    status = psa_import_key(&attributes, key_256, 32, &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Encrypt
    status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_CBC_NO_PADDING);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, nonce, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_update(&operation, plaintext, SZ, ciphertext, SZ, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len = output_len;

    status = psa_cipher_finish(&operation, ciphertext + output_len, SZ - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len += output_len;

    TEST_ASSERT_EQUAL(SZ, total_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_cbc_end, ciphertext + SZ - 32, 32);

    // Decrypt
    psa_cipher_abort(&operation);
    operation = (psa_cipher_operation_t)PSA_CIPHER_OPERATION_INIT;
    memcpy(nonce, iv, 16);

    status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_CBC_NO_PADDING);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, nonce, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_update(&operation, ciphertext, SZ, decryptedtext, SZ, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len = output_len;

    status = psa_cipher_finish(&operation, decryptedtext + output_len, SZ - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len += output_len;

    TEST_ASSERT_EQUAL(SZ, total_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    psa_destroy_key(key_id);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls CBC AES-256 test", "[aes]")
{
    aes_cbc_test(TEST_AES_CBC_DMA_MODE_LEN);
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

TEST_CASE("mbedtls CBC AES-256 DMA buffer align test", "[aes]")
{
// Size is taken considering the maximum DMA buffer size
    const unsigned SZ = ESP_ALIGN_DOWN((2*LLDESC_MAX_NUM_PER_DESC), 16);
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    uint8_t nonce[16];
    psa_status_t status;
    size_t output_len, total_output_len;


    memcpy(nonce, iv, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *plaintext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CBC_NO_PADDING);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 256);

    // Import key
    status = psa_import_key(&attributes, key_256, 32, &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Encrypt
    status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_CBC_NO_PADDING);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, nonce, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_update(&operation, plaintext, SZ, ciphertext, SZ, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len = output_len;

    status = psa_cipher_finish(&operation, ciphertext + output_len, SZ - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len += output_len;

    TEST_ASSERT_EQUAL(SZ, total_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_cbc_align_end, ciphertext + SZ - 32, 32);

    // Decrypt
    psa_cipher_abort(&operation);
    operation = (psa_cipher_operation_t)PSA_CIPHER_OPERATION_INIT;
    memcpy(nonce, iv, 16);

    status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_CBC_NO_PADDING);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, nonce, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_update(&operation, ciphertext, SZ, decryptedtext, SZ, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len = output_len;

    status = psa_cipher_finish(&operation, decryptedtext + output_len, SZ - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len += output_len;

    TEST_ASSERT_EQUAL(SZ, total_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    psa_destroy_key(key_id);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

static void aes_ctr_test(unsigned int SZ)
{
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    uint8_t nonce[16];
    psa_status_t status;
    size_t output_len, total_output_len;


    memcpy(nonce, iv, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *plaintext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CTR);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 256);

    // Import key
    status = psa_import_key(&attributes, key_256, 32, &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Encrypt
    status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_CTR);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, nonce, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_update(&operation, plaintext, SZ, ciphertext, SZ, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len = output_len;

    status = psa_cipher_finish(&operation, ciphertext + output_len, SZ - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len += output_len;

    TEST_ASSERT_EQUAL(SZ, total_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_ctr_end, ciphertext + SZ - 32, 32);

    // Decrypt
    psa_cipher_abort(&operation);
    operation = (psa_cipher_operation_t)PSA_CIPHER_OPERATION_INIT;
    memcpy(nonce, iv, 16);

    status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_CTR);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, nonce, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_update(&operation, ciphertext, SZ, decryptedtext, SZ, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len = output_len;

    status = psa_cipher_finish(&operation, decryptedtext + output_len, SZ - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len += output_len;

    TEST_ASSERT_EQUAL(SZ, total_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    psa_destroy_key(key_id);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls CTR AES-256 test", "[aes]")
{
    aes_ctr_test(TEST_AES_CTR_DMA_MODE_LEN);
}

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

static void aes_ofb_test(unsigned int SZ)
{
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    uint8_t nonce[16];
    psa_status_t status;
    size_t output_len, total_output_len;


    memcpy(nonce, iv, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *plaintext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_OFB);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 256);

    // Import key
    status = psa_import_key(&attributes, key_256, 32, &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Encrypt
    status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_OFB);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, nonce, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_update(&operation, plaintext, SZ, ciphertext, SZ, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len = output_len;

    status = psa_cipher_finish(&operation, ciphertext + output_len, SZ - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len += output_len;

    TEST_ASSERT_EQUAL(SZ, total_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_ofb_end, ciphertext + SZ - 32, 32);

    // Decrypt
    psa_cipher_abort(&operation);
    operation = (psa_cipher_operation_t)PSA_CIPHER_OPERATION_INIT;
    memcpy(nonce, iv, 16);

    status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_OFB);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, nonce, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_update(&operation, ciphertext, SZ, decryptedtext, SZ, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len = output_len;

    status = psa_cipher_finish(&operation, decryptedtext + output_len, SZ - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len += output_len;

    TEST_ASSERT_EQUAL(SZ, total_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    psa_destroy_key(key_id);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls OFB AES-256 test", "[aes]")
{
    aes_ofb_test(TEST_AES_OFB_DMA_MODE_LEN);
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

// CFB8 is not supported by the PSA Crypto API
#if 0
static void aes_cfb8_test(unsigned int SZ)
{
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    uint8_t nonce[16];
    psa_status_t status;
    size_t output_len, total_output_len;


    memcpy(nonce, iv, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *plaintext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CFB);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 256);

    // Import key
    status = psa_import_key(&attributes, key_256, 32, &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Encrypt
    status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_CFB);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, nonce, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_update(&operation, plaintext, SZ, ciphertext, SZ, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len = output_len;

    status = psa_cipher_finish(&operation, ciphertext + output_len, SZ - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len += output_len;

    TEST_ASSERT_EQUAL(SZ, total_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_cfb8_end, ciphertext + SZ - 32, 32);

    // Decrypt
    psa_cipher_abort(&operation);
    operation = (psa_cipher_operation_t)PSA_CIPHER_OPERATION_INIT;
    memcpy(nonce, iv, 16);

    status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_CFB);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, nonce, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_update(&operation, ciphertext, SZ, decryptedtext, SZ, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len = output_len;

    status = psa_cipher_finish(&operation, decryptedtext + output_len, SZ - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len += output_len;

    TEST_ASSERT_EQUAL(SZ, total_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    psa_destroy_key(key_id);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}
TEST_CASE("mbedtls CFB-8 AES-256 test", "[aes]")
{
    aes_cfb8_test(TEST_AES_CFB8_DMA_MODE_LEN);
}
#endif

static void aes_cfb128_test(unsigned int SZ)
{
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    uint8_t nonce[16];
    psa_status_t status;
    size_t output_len, total_output_len;


    memcpy(nonce, iv, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *plaintext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CFB);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 256);

    // Import key
    status = psa_import_key(&attributes, key_256, 32, &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Encrypt
    status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_CFB);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, nonce, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_update(&operation, plaintext, SZ, ciphertext, SZ, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len = output_len;

    status = psa_cipher_finish(&operation, ciphertext + output_len, SZ - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len += output_len;

    TEST_ASSERT_EQUAL(SZ, total_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_cfb128_end, ciphertext + SZ - 32, 32);

    // Decrypt
    psa_cipher_abort(&operation);
    operation = (psa_cipher_operation_t)PSA_CIPHER_OPERATION_INIT;
    memcpy(nonce, iv, 16);

    status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_CFB);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, nonce, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_update(&operation, ciphertext, SZ, decryptedtext, SZ, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len = output_len;

    status = psa_cipher_finish(&operation, decryptedtext + output_len, SZ - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_output_len += output_len;

    TEST_ASSERT_EQUAL(SZ, total_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    psa_destroy_key(key_id);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls CFB-128 AES-256 test", "[aes]")
{
    aes_cfb128_test(TEST_AES_CFB128_DMA_MODE_LEN);
}

static void aes_ctr_stream_test(uint32_t input_buf_caps, uint32_t output_buf_caps, unsigned int SZ)
{
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t nonce[16];
    uint8_t key[16];
    psa_status_t status;


    memset(nonce, 0xEE, 16);
    memset(key, 0x44, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, input_buf_caps);
    uint8_t *plaintext = heap_caps_malloc(SZ, output_buf_caps);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0xAA, SZ);

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CTR);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    // Import key
    status = psa_import_key(&attributes, key, 16, &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    /* Test that all the end results are the same
        no matter how many bytes we encrypt each call
        */
    for (int bytes_to_process = 1; bytes_to_process < SZ; bytes_to_process++) {
        ESP_LOGD("test", "bytes_to_process %d", bytes_to_process);
        memset(nonce, 0xEE, 16);
        memset(ciphertext, 0x0, SZ);
        memset(decryptedtext, 0x0, SZ);

        size_t output_offset = 0;
        size_t output_len;
        psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;

        // Encrypt
        status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_CTR);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        status = psa_cipher_set_iv(&operation, nonce, 16);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        for (int idx = 0; idx < SZ; idx = idx + bytes_to_process) {
            // Limit length of last call to avoid exceeding buffer size
            size_t length = (idx + bytes_to_process > SZ) ? (SZ - idx) : bytes_to_process;

            status = psa_cipher_update(&operation, plaintext + idx, length,
                                      ciphertext + output_offset, SZ - output_offset, &output_len);
            TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
            output_offset += output_len;
        }

        status = psa_cipher_finish(&operation, ciphertext + output_offset, SZ - output_offset, &output_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        output_offset += output_len;

        ESP_LOG_BUFFER_HEXDUMP("expected", expected_cipher_ctr_stream, SZ, ESP_LOG_DEBUG);
        ESP_LOG_BUFFER_HEXDUMP("actual  ", ciphertext, SZ, ESP_LOG_DEBUG);

        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_ctr_stream, ciphertext, SZ);

        // Decrypt
        memset(nonce, 0xEE, 16);
        memset(decryptedtext, 0x22, SZ);
        output_offset = 0;

        psa_cipher_abort(&operation);
        operation = (psa_cipher_operation_t)PSA_CIPHER_OPERATION_INIT;
        status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_CTR);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        status = psa_cipher_set_iv(&operation, nonce, 16);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        for (int idx = 0; idx < SZ; idx = idx + bytes_to_process) {
            // Limit length of last call to avoid exceeding buffer size
            size_t length = (idx + bytes_to_process > SZ) ? (SZ - idx) : bytes_to_process;

            status = psa_cipher_update(&operation, ciphertext + idx, length,
                                      decryptedtext + output_offset, SZ - output_offset, &output_len);
            TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
            output_offset += output_len;
        }

        status = psa_cipher_finish(&operation, decryptedtext + output_offset, SZ - output_offset, &output_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        output_offset += output_len;

        ESP_LOG_BUFFER_HEXDUMP("decrypted", decryptedtext, SZ, ESP_LOG_DEBUG);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);
    }

    psa_destroy_key(key_id);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls CTR stream test", "[aes]")
{
    aes_ctr_stream_test(INTERNAL_DMA_CAPS, INTERNAL_DMA_CAPS, TEST_AES_CTR_STREAM_DMA_MODE_LEN);
}

static void aes_ofb_stream_test(unsigned int SZ)
{
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t iv[16];
    uint8_t key[16];
    psa_status_t status;


    memset(key, 0x44, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *plaintext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0xAA, SZ);

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_OFB);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    // Import key
    status = psa_import_key(&attributes, key, 16, &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    /* Test that all the end results are the same
        no matter how many bytes we encrypt each call
        */

    for (int bytes_to_process = 1; bytes_to_process < SZ; bytes_to_process++) {
        ESP_LOGD("test", "bytes_to_process %d", bytes_to_process);
        // Encrypt
        memset(iv, 0xEE, 16);
        size_t output_offset = 0;
        size_t output_len;
        psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;

        status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_OFB);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        status = psa_cipher_set_iv(&operation, iv, 16);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        for (int idx = 0; idx < SZ; idx = idx + bytes_to_process) {
            // Limit length of last call to avoid exceeding buffer size
            size_t length = ( (idx + bytes_to_process) > SZ) ? (SZ - idx) : bytes_to_process;

            status = psa_cipher_update(&operation, plaintext + idx, length,
                                      ciphertext + output_offset, SZ - output_offset, &output_len);
            TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
            output_offset += output_len;
        }

        status = psa_cipher_finish(&operation, ciphertext + output_offset, SZ - output_offset, &output_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        output_offset += output_len;

        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_ofb_stream, ciphertext, SZ);

        // Decrypt
        memset(iv, 0xEE, 16);
        memset(decryptedtext, 0x22, SZ);
        output_offset = 0;

        psa_cipher_abort(&operation);
        operation = (psa_cipher_operation_t)PSA_CIPHER_OPERATION_INIT;
        status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_OFB);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        status = psa_cipher_set_iv(&operation, iv, 16);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        for (int idx = 0; idx < SZ; idx = idx + bytes_to_process) {
            // Limit length of last call to avoid exceeding buffer size
            size_t length = (idx + bytes_to_process > SZ) ? (SZ - idx) : bytes_to_process;

            status = psa_cipher_update(&operation, ciphertext + idx, length,
                                      decryptedtext + output_offset, SZ - output_offset, &output_len);
            TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
            output_offset += output_len;
        }

        status = psa_cipher_finish(&operation, decryptedtext + output_offset, SZ - output_offset, &output_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        output_offset += output_len;

        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);
    }

    psa_destroy_key(key_id);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls OFB stream test", "[aes]")
{
    aes_ofb_stream_test(TEST_AES_OFB_STREAM_DMA_MODE_LEN);
}

#if 0
static void aes_cfb8_stream_test(unsigned int SZ)
{
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t iv[16];
    uint8_t key[16];
    psa_status_t status;


    memset(key, 0x44, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *plaintext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0xAA, SZ);

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CFB);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    // Import key
    status = psa_import_key(&attributes, key, 16, &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    /* Test that all the end results are the same
        no matter how many bytes we encrypt each call
        */

    for (int bytes_to_process = 1; bytes_to_process < SZ; bytes_to_process++) {
        memset(iv, 0xEE, 16);
        size_t output_offset = 0;
        size_t output_len;
        psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;

        status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_CFB);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        status = psa_cipher_set_iv(&operation, iv, 16);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        for (int idx = 0; idx < SZ; idx = idx + bytes_to_process) {
            // Limit length of last call to avoid exceeding buffer size
            size_t length = ( (idx + bytes_to_process) > SZ) ? (SZ - idx) : bytes_to_process;

            status = psa_cipher_update(&operation, plaintext + idx, length,
                                      ciphertext + output_offset, SZ - output_offset, &output_len);
            TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
            output_offset += output_len;
        }

        status = psa_cipher_finish(&operation, ciphertext + output_offset, SZ - output_offset, &output_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        output_offset += output_len;

        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_cfb8_stream, ciphertext, SZ);

        memset(iv, 0xEE, 16);
        output_offset = 0;

        psa_cipher_abort(&operation);
        operation = (psa_cipher_operation_t)PSA_CIPHER_OPERATION_INIT;
        status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_CFB);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        status = psa_cipher_set_iv(&operation, iv, 16);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        for (int idx = 0; idx < SZ; idx = idx + bytes_to_process) {
            // Limit length of last call to avoid exceeding buffer size
            size_t length = ( (idx + bytes_to_process) > SZ) ? (SZ - idx) : bytes_to_process;

            status = psa_cipher_update(&operation, ciphertext + idx, length,
                                      decryptedtext + output_offset, SZ - output_offset, &output_len);
            TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
            output_offset += output_len;
        }

        status = psa_cipher_finish(&operation, decryptedtext + output_offset, SZ - output_offset, &output_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        output_offset += output_len;

        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);
    }

    psa_destroy_key(key_id);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls CFB8 stream test", "[aes]")
{
    aes_cfb8_stream_test(TEST_AES_CFB8_STREAM_DMA_MODE_LEN);
}
#endif

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

static void aes_cfb128_stream_test(unsigned int SZ)
{
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t iv[16];
    uint8_t key[16];
    psa_status_t status;


    memset(key, 0x44, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *plaintext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0xAA, SZ);

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CFB);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    // Import key
    status = psa_import_key(&attributes, key, 16, &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    /* Test that all the end results are the same
        no matter how many bytes we encrypt each call
    */

    //for (int bytes_to_process = 1; bytes_to_process < SZ; bytes_to_process++) {
    int bytes_to_process = 17;
    size_t output_offset = 0;
    size_t output_len;
    memset(iv, 0xEE, 16);

    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_CFB);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, iv, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    for (int idx = 0; idx < SZ; idx = idx + bytes_to_process) {
        // Limit length of last call to avoid exceeding buffer size
        size_t length = ( (idx + bytes_to_process) > SZ) ? (SZ - idx) : bytes_to_process;

        status = psa_cipher_update(&operation, plaintext + idx, length,
                                  ciphertext + output_offset, SZ - output_offset, &output_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        output_offset += output_len;
    }

    status = psa_cipher_finish(&operation, ciphertext + output_offset, SZ - output_offset, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    output_offset += output_len;

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_cfb128_stream, ciphertext, SZ);

    output_offset = 0;
    memset(iv, 0xEE, 16);

    psa_cipher_abort(&operation);
    operation = (psa_cipher_operation_t)PSA_CIPHER_OPERATION_INIT;
    status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_CFB);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, iv, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    for (int idx = 0; idx < SZ; idx = idx + bytes_to_process) {
        // Limit length of last call to avoid exceeding buffer size
        size_t length = ( (idx + bytes_to_process) > SZ) ? (SZ - idx) : bytes_to_process;

        status = psa_cipher_update(&operation, ciphertext + idx, length,
                                  decryptedtext + output_offset, SZ - output_offset, &output_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        output_offset += output_len;
    }

    status = psa_cipher_finish(&operation, decryptedtext + output_offset, SZ - output_offset, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    output_offset += output_len;

    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    psa_destroy_key(key_id);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls CFB128 stream test", "[aes]")
{
    aes_cfb128_stream_test(TEST_AES_CFB128_DMA_MODE_LEN);
}


#if CONFIG_MBEDTLS_HARDWARE_AES
/* Test the case where the input and output buffers point to the same location */
/* As, mbedtls does not support in-place encryption for block cipher modes, hence this test is only applicable for hardware AES */
TEST_CASE("mbedtls CTR, input buf = output buf", "[aes]")
{
    const unsigned SZ = 1000;
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    uint8_t nonce[16];
    psa_status_t status;
    size_t output_len;


    memcpy(nonce, iv, 16);

    // allocate internal memory
    uint8_t *buf = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);

    TEST_ASSERT_NOT_NULL(buf);

    memset(buf, 0x3A, SZ);

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CTR);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 256);

    // Import key
    status = psa_import_key(&attributes, key_256, 32, &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Encrypt
    status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_CTR);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, nonce, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    size_t total_len = 0;
    status = psa_cipher_update(&operation, buf, SZ, buf, SZ, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_len += output_len;

    status = psa_cipher_finish(&operation, buf + output_len, SZ - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_len += output_len;
    TEST_ASSERT_EQUAL(SZ, total_len);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_ctr_inplace_end, buf + SZ - 32, 32);

    // Decrypt
    memcpy(nonce, iv, 16);

    psa_cipher_abort(&operation);
    operation = (psa_cipher_operation_t)PSA_CIPHER_OPERATION_INIT;
    status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_CTR);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, nonce, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    total_len = 0;
    status = psa_cipher_update(&operation, buf, SZ, buf, SZ, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_len += output_len;

    status = psa_cipher_finish(&operation, buf + output_len, SZ - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_len += output_len;
    TEST_ASSERT_EQUAL(SZ, total_len);

    for (int i = 0; i < SZ; i++) {
        TEST_ASSERT_EQUAL_HEX8(0x3A, buf[i]);
    }

    psa_destroy_key(key_id);
    free(buf);
}
#endif /* CONFIG_MBEDTLS_HARDWARE_AES */

TEST_CASE("mbedtls OFB, chained DMA descriptors", "[aes]")
{
    // Max bytes in a single DMA descriptor is 4095
    const unsigned SZ = 6000;
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    uint8_t nonce[16];
    psa_status_t status;
    size_t output_len;


    memcpy(nonce, iv, 16);

    // allocate internal memory
    uint8_t *ciphertext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *plaintext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x3A, SZ);
    memset(decryptedtext, 0x0, SZ);

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_OFB);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 256);

    // Import key
    status = psa_import_key(&attributes, key_256, 32, &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Encrypt
    status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_OFB);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, nonce, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    size_t total_len = 0;
    status = psa_cipher_update(&operation, plaintext, SZ, ciphertext, SZ, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_len += output_len;

    status = psa_cipher_finish(&operation, ciphertext + output_len, SZ - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_len += output_len;
    TEST_ASSERT_EQUAL(SZ, total_len);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_ofb_chained_end, ciphertext + SZ - 32, 32);

    // Decrypt
    memcpy(nonce, iv, 16);

    psa_cipher_abort(&operation);
    operation = (psa_cipher_operation_t)PSA_CIPHER_OPERATION_INIT;
    status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_OFB);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, nonce, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    total_len = 0;
    status = psa_cipher_update(&operation, ciphertext, SZ, decryptedtext, SZ, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_len += output_len;

    status = psa_cipher_finish(&operation, decryptedtext + output_len, SZ - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_len += output_len;
    TEST_ASSERT_EQUAL(SZ, total_len);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

    psa_destroy_key(key_id);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

void aes_ctr_alignment_test(uint32_t input_buf_caps, uint32_t output_buf_caps)
{
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t nonce[16];
    uint8_t key[16];
    psa_status_t status;
    size_t SZ = TEST_AES_CTR_DATA_LEN;
    size_t ALIGNMENT_SIZE_BYTES = 64;
    memset(nonce, 0x2F, 16);
    memset(key, 0x1E, 16);

    // allocate memory according the requested caps
    uint8_t *ciphertext = heap_caps_malloc(SZ + ALIGNMENT_SIZE_BYTES, output_buf_caps);
    uint8_t *plaintext = heap_caps_malloc(SZ + ALIGNMENT_SIZE_BYTES, input_buf_caps);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    memset(plaintext, 0x26, SZ + ALIGNMENT_SIZE_BYTES);

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CTR);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    // Import key
    status = psa_import_key(&attributes, key, 16, &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    size_t output_len;

    /* Shift buffers and test for all different misalignments */
    for (int i = 0; i < ALIGNMENT_SIZE_BYTES; i++ ) {
        // Encrypt with input buffer in external ram
        memset(nonce, 0x2F, 16);
        psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;

        status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_CTR);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        status = psa_cipher_set_iv(&operation, nonce, 16);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        size_t total_len = 0;
        status = psa_cipher_update(&operation, plaintext + i, SZ, ciphertext + i, SZ, &output_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        total_len += output_len;

        status = psa_cipher_finish(&operation, ciphertext + i + output_len, SZ - output_len, &output_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        total_len += output_len;
        TEST_ASSERT_EQUAL(SZ, total_len);

        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_ctr, ciphertext + i, SZ);

        // Decrypt
        memset(nonce, 0x2F, 16);

        psa_cipher_abort(&operation);
        operation = (psa_cipher_operation_t)PSA_CIPHER_OPERATION_INIT;
        status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_CTR);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        status = psa_cipher_set_iv(&operation, nonce, 16);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        total_len = 0;
        status = psa_cipher_update(&operation, ciphertext + i, SZ, decryptedtext, SZ, &output_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        total_len += output_len;

        status = psa_cipher_finish(&operation, decryptedtext + output_len, SZ - output_len, &output_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        total_len += output_len;
        TEST_ASSERT_EQUAL(SZ, total_len);

        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);
    }

    psa_destroy_key(key_id);
    free(plaintext);
    free(ciphertext);
    free(decryptedtext);
}

TEST_CASE("mbedtls AES internal mem alignment tests", "[aes]")
{
    uint32_t internal_dma_caps = INTERNAL_DMA_CAPS;
    aes_ctr_alignment_test(internal_dma_caps, internal_dma_caps);
}

#ifdef CONFIG_SPIRAM_USE_MALLOC

void aes_psram_one_buf_ctr_test(void)
{
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t nonce[16];
    uint8_t key[16];
    psa_status_t status;
    size_t SZ = TEST_AES_CTR_DATA_LEN;
    size_t ALIGNMENT_SIZE_BYTES = 32;
    memset(nonce, 0x2F, 16);
    memset(key, 0x1E, 16);

    // allocate external memory
    uint8_t *buf = heap_caps_malloc(SZ + ALIGNMENT_SIZE_BYTES, PSRAM_DMA_CAPS);

    TEST_ASSERT_NOT_NULL(buf);

    memset(buf, 0x26, SZ + ALIGNMENT_SIZE_BYTES);

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CTR);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    // Import key
    status = psa_import_key(&attributes, key, 16, &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    size_t output_len;

    /* Shift buffers and test for all different misalignments */
    for (int i = 0; i < ALIGNMENT_SIZE_BYTES; i++ ) {
        // Encrypt with input buffer in external ram
        memset(buf, 0x26, SZ + ALIGNMENT_SIZE_BYTES);
        memset(nonce, 0x2F, 16);
        psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;

        status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_CTR);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        status = psa_cipher_set_iv(&operation, nonce, 16);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        size_t total_len = 0;
        status = psa_cipher_update(&operation, buf + i, SZ, buf + i, SZ, &output_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        total_len += output_len;

        status = psa_cipher_finish(&operation, buf + i + output_len, SZ - output_len, &output_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        total_len += output_len;
        TEST_ASSERT_EQUAL(SZ, total_len);

        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_ctr, buf + i, SZ);

        // Decrypt
        memset(nonce, 0x2F, 16);

        psa_cipher_abort(&operation);
        operation = (psa_cipher_operation_t)PSA_CIPHER_OPERATION_INIT;
        status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_CTR);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        status = psa_cipher_set_iv(&operation, nonce, 16);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        total_len = 0;
        status = psa_cipher_update(&operation, buf + i, SZ, buf, SZ, &output_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        total_len += output_len;

        status = psa_cipher_finish(&operation, buf + output_len, SZ - output_len, &output_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        total_len += output_len;
        TEST_ASSERT_EQUAL(SZ, total_len);

        TEST_ASSERT_EACH_EQUAL_HEX8(0x26, buf + i, SZ - i);
    }

    psa_destroy_key(key_id);
    free(buf);
}

void aes_ext_flash_ctr_test(uint32_t output_buf_caps)
{
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t nonce[16];
    uint8_t key[16];
    psa_status_t status;
    size_t SZ = sizeof(long_input);
    memset(nonce, 0x2F, 16);
    memset(key, 0x1E, 16);

    uint8_t *ciphertext = heap_caps_malloc(SZ, output_buf_caps);
    uint8_t *decryptedtext = heap_caps_malloc(SZ, INTERNAL_DMA_CAPS);

    TEST_ASSERT_NOT_NULL(ciphertext);
    TEST_ASSERT_NOT_NULL(decryptedtext);

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CTR);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    // Import key
    status = psa_import_key(&attributes, key, 16, &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    size_t output_len;

    // Encrypt with input buffer in external flash
    memset(nonce, 0x2F, 16);

    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_CTR);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, nonce, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    size_t total_len = 0;
    status = psa_cipher_update(&operation, long_input, SZ, ciphertext, SZ, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_len += output_len;

    status = psa_cipher_finish(&operation, ciphertext + output_len, SZ - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_len += output_len;
    TEST_ASSERT_EQUAL(SZ, total_len);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_long_input_end, ciphertext + SZ - 32, 32);

    // Decrypt
    memset(nonce, 0x2F, 16);

    psa_cipher_abort(&operation);
    operation = (psa_cipher_operation_t)PSA_CIPHER_OPERATION_INIT;
    status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_CTR);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_cipher_set_iv(&operation, nonce, 16);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    total_len = 0;
    status = psa_cipher_update(&operation, ciphertext, SZ, decryptedtext, SZ, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_len += output_len;

    status = psa_cipher_finish(&operation, decryptedtext + output_len, SZ - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_len += output_len;
    TEST_ASSERT_EQUAL(SZ, total_len);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(long_input, decryptedtext, SZ);

    psa_destroy_key(key_id);
    free(ciphertext);
    free(decryptedtext);
}

/* Tests how crypto DMA handles data in external memory */
TEST_CASE("mbedtls AES PSRAM tests", "[aes]")
{
    aes_ctr_alignment_test(INTERNAL_DMA_CAPS, PSRAM_DMA_CAPS);
    aes_ctr_alignment_test(PSRAM_DMA_CAPS, INTERNAL_DMA_CAPS);
    aes_ctr_alignment_test(PSRAM_DMA_CAPS, PSRAM_DMA_CAPS);
    aes_psram_one_buf_ctr_test();
    aes_ctr_stream_test(INTERNAL_DMA_CAPS, PSRAM_DMA_CAPS, TEST_AES_CTR_STREAM_DMA_MODE_LEN);
    aes_ctr_stream_test(PSRAM_DMA_CAPS, INTERNAL_DMA_CAPS, TEST_AES_CTR_STREAM_DMA_MODE_LEN);
    aes_ctr_stream_test(PSRAM_DMA_CAPS, PSRAM_DMA_CAPS, TEST_AES_CTR_STREAM_DMA_MODE_LEN);
}

/* Tests how crypto DMA handles data from external flash */
TEST_CASE("mbedtls AES external flash tests", "[aes]")
{
    aes_ext_flash_ctr_test(PSRAM_DMA_CAPS);
    aes_ext_flash_ctr_test(INTERNAL_DMA_CAPS);
}
#endif // CONFIG_SPIRAM_USE_MALLOC

static SemaphoreHandle_t done_sem;

static void __attribute__((unused)) aes_ctr_stream_test_task(void *pv)
{
    aes_ctr_stream_test(INTERNAL_DMA_CAPS, INTERNAL_DMA_CAPS, TEST_AES_CTR_STREAM_DMA_MODE_LEN);
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

#if CONFIG_ESP_SYSTEM_RTC_FAST_MEM_AS_HEAP_DEPCHECK && !CONFIG_IDF_TARGET_ESP32H2
// Not enough rtc memory for test on H2

TEST_CASE("mbedtls AES stack in RTC RAM", "[mbedtls]")
{
    done_sem = xSemaphoreCreateBinary();
    static StaticTask_t rtc_task;
    size_t STACK_SIZE = 3072;
    uint8_t *rtc_stack = heap_caps_calloc(STACK_SIZE, 1, MALLOC_CAP_RTCRAM);
    TEST_ASSERT(esp_ptr_in_rtc_dram_fast(rtc_stack));

    TEST_ASSERT_NOT_NULL(xTaskCreateStatic(aes_ctr_stream_test_task, "aes_ctr_task", STACK_SIZE, NULL,
                                            3, rtc_stack, &rtc_task));
    TEST_ASSERT_TRUE(xSemaphoreTake(done_sem, 10000 / portTICK_PERIOD_MS));

    /* Give task time to cleanup before freeing stack */
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    free(rtc_stack);

    vSemaphoreDelete(done_sem);
}

#endif //CONFIG_ESP_SYSTEM_RTC_FAST_MEM_AS_HEAP_DEPCHECK && !CONFIG_IDF_TARGET_ESP32H2

#if CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM && CONFIG_SPIRAM_USE_MALLOC

TEST_CASE("mbedtls AES stack in PSRAM", "[mbedtls]")
{
    done_sem = xSemaphoreCreateBinary();
    static StaticTask_t psram_task;
    size_t STACK_SIZE = 3072;
    uint8_t *psram_stack = heap_caps_calloc(STACK_SIZE, 1, PSRAM_DMA_CAPS);

    TEST_ASSERT(esp_ptr_external_ram(psram_stack));

    TEST_ASSERT_NOT_NULL(xTaskCreateStatic(aes_ctr_stream_test_task, "aes_ctr_task", STACK_SIZE, NULL,
                                            3, psram_stack, &psram_task));
    TEST_ASSERT_TRUE(xSemaphoreTake(done_sem, 10000 / portTICK_PERIOD_MS));

    /* Give task time to cleanup before freeing stack */
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    free(psram_stack);

    vSemaphoreDelete(done_sem);
}

#endif //CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM && CONFIG_SPIRAM_USE_MALLOC

static const uint8_t key_192[24] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
};

TEST_CASE("PSA AES-CBC-192", "[psa-aes]")
{
    const size_t SZ = 1008;
    const size_t iv_SZ = 16;


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
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_cbc192_end, ciphertext + SZ - sizeof(expected_cipher_cbc192_end), sizeof(expected_cipher_cbc192_end));

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
