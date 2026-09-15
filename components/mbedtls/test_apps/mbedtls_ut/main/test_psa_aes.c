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

/* AES-256-ECB of plaintext[i] = i * 7 + 1 under key_256, taken from OpenSSL
 * rather than from this library, so that an implementation which is
 * consistently wrong cannot satisfy the test:
 *
 *     openssl enc -aes-256-ecb -nopad \
 *         -K 000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f
 */
static const uint8_t ecb_kat_256[112] = {
    0x95, 0xc9, 0x03, 0x0d, 0x4a, 0xca, 0x42, 0x59,
    0x7e, 0x1f, 0x19, 0x9a, 0x95, 0xe1, 0xa5, 0xe8,
    0x20, 0x28, 0x07, 0x2a, 0xe8, 0xf3, 0x51, 0x29,
    0x6d, 0x4d, 0x31, 0xb9, 0xe3, 0xb1, 0xc2, 0x17,
    0x89, 0x75, 0xd2, 0xb7, 0x77, 0x38, 0x93, 0x6b,
    0x1c, 0x52, 0x6f, 0x57, 0x9d, 0x11, 0x67, 0x9c,
    0xc1, 0x68, 0x42, 0x46, 0xc4, 0x76, 0x3e, 0x6c,
    0xef, 0x37, 0xa3, 0xd0, 0x1f, 0x2c, 0x24, 0x30,
    0xe7, 0xe2, 0x56, 0x2d, 0x6e, 0xa5, 0x21, 0x85,
    0x9c, 0x64, 0x21, 0x8d, 0x77, 0xe2, 0xfe, 0xa9,
    0xc3, 0xe6, 0x87, 0xde, 0x3b, 0xac, 0x65, 0xb5,
    0x94, 0x7a, 0x34, 0xc7, 0x24, 0xd1, 0xe5, 0x6a,
    0x8c, 0x50, 0xeb, 0xa7, 0x69, 0xed, 0x47, 0xfc,
    0x48, 0xfa, 0xe4, 0x00, 0x78, 0xbc, 0x7f, 0x29,
};

/* PSA lets a caller split a message across update() calls at any byte boundary,
 * so an update can start with a partial block held over from the call before it.
 * Completing that block writes a whole block while consuming fewer than a block
 * of input, which leaves the output cursor ahead of the input cursor for the
 * rest of the call: the writes land on input bytes that have not been read yet,
 * and there is no correct result to produce.
 *
 * The table below pins both sides of that. The overlaps which must still be
 * honoured have to keep working, and the ones which cannot be honoured have to
 * be refused rather than quietly producing wrong ciphertext.
 */
TEST_CASE("PSA AES-ECB in-place overlap", "[psa-aes]")
{
    const size_t SZ = 128;
    const size_t PAD = 16;
    const uint8_t canary = 0xA5;

    uint8_t *plaintext = malloc(SZ);
    uint8_t *reference = malloc(SZ);
    uint8_t *scratch = malloc(SZ);
    /* DMA-capable internal memory: on the hardware path this buffer is handed
       to the AES accelerator as both source and destination. The PAD bytes of
       slack hold a canary, so a write past the ciphertext fails an assertion
       here instead of damaging the heap somewhere else. */
    uint8_t *buf = heap_caps_malloc(SZ + PAD, INTERNAL_DMA_CAPS);

    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(reference);
    TEST_ASSERT_NOT_NULL(scratch);
    TEST_ASSERT_NOT_NULL(buf);

    /* Every block must differ, otherwise a bug that reordered, repeated or
       dropped blocks would produce identical ciphertext and go unnoticed. */
    for (size_t i = 0; i < SZ; i++) {
        plaintext[i] = (uint8_t)(i * 7 + 1);
    }

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_crypto_init());

    psa_key_id_t key_id;
    psa_algorithm_t alg = PSA_ALG_ECB_NO_PADDING;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, sizeof(key_256) * 8);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_import_key(&attributes, key_256, sizeof(key_256), &key_id));
    psa_reset_key_attributes(&attributes);

    psa_cipher_operation_t op = PSA_CIPHER_OPERATION_INIT;
    psa_status_t status;
    size_t out_len;

    /* Reference, from separate buffers, anchored to the OpenSSL vector so that
       everything compared against it below is trustworthy. */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_encrypt_setup(&op, key_id, alg));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&op, plaintext, SZ, reference, SZ, &out_len));
    TEST_ASSERT_EQUAL_size_t(SZ, out_len);
    psa_cipher_abort(&op);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(ecb_kat_256, reference, sizeof(ecb_kat_256));

    /* In-place, with the second call's output at a chosen distance from its
       input: the overlap that must still be honoured, the one that corrupts,
       the one a check on output == input alone would let through, and output
       placed past everything the call reads. */
    static const struct {
        const char *name;
        size_t first;
        size_t second;
        int delta;
        bool safe;
    } overlap_cases[] = {
        { "output == input, nothing pending",     32, 32,  0, true  },
        { "output == input, one byte pending",    33, 31,  0, false },
        { "output == input + 1, nothing pending", 32, 32,  1, false },
        { "output past the end of the input",     32, 32, 32, true  },
    };

#if CONFIG_MBEDTLS_HARDWARE_AES
    const psa_status_t unsafe_status = PSA_ERROR_INVALID_ARGUMENT;
#else
    /* The software path rejects these too once the mbedtls submodule carries
       the same ECB overlap check. Until then it accepts and mis-frames the
       output. Drop this branch with the submodule bump. */
    const psa_status_t unsafe_status = PSA_SUCCESS;
#endif

    for (size_t c = 0; c < sizeof(overlap_cases) / sizeof(overlap_cases[0]); c++) {
        const size_t first = overlap_cases[c].first;
        const size_t second = overlap_cases[c].second;
        size_t len1, pending, expected_len;
        uint8_t *in2, *out2;

        memcpy(buf, plaintext, SZ);
        memset(buf + SZ, canary, PAD);
        op = (psa_cipher_operation_t)PSA_CIPHER_OPERATION_INIT;
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_encrypt_setup(&op, key_id, alg));

        /* Exactly in place with nothing pending is always supported, and the
           size of this call decides what the next one has to carry. */
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&op, buf, first, buf,
                                                         SZ + PAD, &len1));
        TEST_ASSERT_EQUAL_size_t(first / 16 * 16, len1);
        pending = first - len1;

        in2 = buf + first;
        out2 = in2 + overlap_cases[c].delta;
        expected_len = (pending + second) / 16 * 16;

        /* Rejected calls must leave the buffer exactly as it is now. */
        memcpy(scratch, buf, SZ);

        out_len = 0;
        status = psa_cipher_update(&op, in2, second, out2,
                                   SZ + PAD - (size_t)(out2 - buf), &out_len);

        if (overlap_cases[c].safe) {
            TEST_ASSERT_EQUAL_MESSAGE(PSA_SUCCESS, status, overlap_cases[c].name);
            TEST_ASSERT_EQUAL_size_t(expected_len, out_len);
            /* This call continues the stream, so what it writes is the
               ciphertext that follows whatever the first call produced. */
            if (out_len != 0) {
                TEST_ASSERT_EQUAL_HEX8_ARRAY(reference + len1, out2, out_len);
            }
        } else {
            TEST_ASSERT_EQUAL_MESSAGE(unsafe_status, status, overlap_cases[c].name);
            if (status != PSA_SUCCESS) {
                TEST_ASSERT_EQUAL_size_t(0, out_len);
                TEST_ASSERT_EQUAL_HEX8_ARRAY(scratch, buf, SZ);
            }
        }

        for (size_t i = SZ; i < SZ + PAD; i++) {
            TEST_ASSERT_EQUAL_HEX8(canary, buf[i]);
        }
        psa_cipher_abort(&op);
    }

    free(plaintext);
    free(reference);
    free(scratch);
    free(buf);
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

static void aes_ctr_crypt_in_parts(psa_key_id_t key_id, const uint8_t *nonce, const uint8_t *input,
                                   uint8_t *output, size_t len, size_t part_size)
{
    psa_cipher_operation_t op = PSA_CIPHER_OPERATION_INIT;
    size_t out_len, total_out_len = 0;

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_encrypt_setup(&op, key_id, PSA_ALG_CTR));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_set_iv(&op, nonce, 16));
    for (size_t offset = 0; offset < len; offset += part_size) {
        size_t this_part = len - offset < part_size ? len - offset : part_size;
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_update(&op, input + offset, this_part,
                                                        output + offset, this_part, &out_len));
        total_out_len += out_len;
    }
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_cipher_finish(&op, output + total_out_len,
                                                    len - total_out_len, &out_len));
    total_out_len += out_len;
    TEST_ASSERT_EQUAL_size_t(len, total_out_len);
    psa_cipher_abort(&op);
}

TEST_CASE("PSA AES-CTR counter carries past the low 32 bits", "[psa-aes]")
{
    const size_t SZ = 1024;
    const size_t small_part = 64;

    static const uint8_t key_128[16] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
    };
    static const uint8_t nonce[16] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0xff, 0xff, 0xff, 0xff,
    };
    static const uint8_t expected_first_blocks[48] = {
        0xbd, 0xb7, 0xc0, 0xef, 0x49, 0x71, 0x79, 0x42, 0xfc, 0x68, 0xee, 0xb1, 0x76, 0x92, 0xfc, 0xf4,
        0xee, 0xf8, 0x9e, 0x94, 0x94, 0xc1, 0x08, 0x2a, 0xb2, 0x7d, 0x4d, 0x90, 0x95, 0xfe, 0xff, 0x60,
        0xe4, 0xc5, 0x5e, 0x02, 0x4d, 0xf3, 0xf2, 0x65, 0xe4, 0x36, 0xab, 0x97, 0x20, 0x92, 0x1b, 0xb4,
    };

    uint8_t *plaintext = heap_caps_calloc(1, SZ, INTERNAL_DMA_CAPS);
    uint8_t *ciphertext_one_part = heap_caps_calloc(1, SZ, INTERNAL_DMA_CAPS);
    uint8_t *ciphertext_small_parts = heap_caps_calloc(1, SZ, INTERNAL_DMA_CAPS);

    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(ciphertext_one_part);
    TEST_ASSERT_NOT_NULL(ciphertext_small_parts);

    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CTR);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_import_key(&attributes, key_128, sizeof(key_128), &key_id));
    psa_reset_key_attributes(&attributes);

    aes_ctr_crypt_in_parts(key_id, nonce, plaintext, ciphertext_one_part, SZ, SZ);
    aes_ctr_crypt_in_parts(key_id, nonce, plaintext, ciphertext_small_parts, SZ, small_part);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_first_blocks, ciphertext_small_parts, sizeof(expected_first_blocks));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_first_blocks, ciphertext_one_part, sizeof(expected_first_blocks));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(ciphertext_small_parts, ciphertext_one_part, SZ);

    psa_destroy_key(key_id);
    free(plaintext);
    free(ciphertext_one_part);
    free(ciphertext_small_parts);
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

#if CONFIG_SPIRAM_ECC_ENABLE && SOC_AES_SUPPORT_DMA
#define TEST_AES_PAYLOAD_LEN 53

struct aes_payload_sim_hdr {
    uint8_t type;
    uint8_t fc;
    uint8_t seq;
    uint8_t len;
    uint8_t data[];
} __attribute__((packed));

static void aes_psram_ecc_cfb128_inplace_test(void)
{
    const size_t pkt_len = sizeof(struct aes_payload_sim_hdr) + TEST_AES_PAYLOAD_LEN;
    struct aes_payload_sim_hdr *pkt = heap_caps_aligned_alloc(16, pkt_len, PSRAM_DMA_CAPS);
    uint8_t *backup = heap_caps_malloc(TEST_AES_PAYLOAD_LEN, INTERNAL_DMA_CAPS);
    uint8_t key[16];
    uint8_t iv[16];
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t status;
    size_t output_len;
    size_t total_len;

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(backup);
    TEST_ASSERT_TRUE(esp_ptr_external_ram(pkt));
    TEST_ASSERT_EQUAL_UINT32(0, (uintptr_t)pkt & 0x0F);
    TEST_ASSERT_EQUAL_UINT32(sizeof(struct aes_payload_sim_hdr) & 0x0F, (uintptr_t)pkt->data & 0x0F);

    pkt->type = 0x13;
    pkt->fc = 0x04;
    pkt->seq = 1;
    pkt->len = TEST_AES_PAYLOAD_LEN;
    for (size_t i = 0; i < TEST_AES_PAYLOAD_LEN; i++) {
        pkt->data[i] = 0x3C + (uint8_t)(i & 0x0F);
    }
    memcpy(backup, pkt->data, TEST_AES_PAYLOAD_LEN);

    memset(key, 0x5A, sizeof(key));
    memset(iv, 0xA5, sizeof(iv));
    iv[0] = 3;

    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CFB);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    status = psa_import_key(&attributes, key, sizeof(key), &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_CFB);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    status = psa_cipher_set_iv(&operation, iv, sizeof(iv));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_len = 0;
    status = psa_cipher_update(&operation, pkt->data, TEST_AES_PAYLOAD_LEN, pkt->data, TEST_AES_PAYLOAD_LEN, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_len += output_len;
    status = psa_cipher_finish(&operation, pkt->data + output_len, TEST_AES_PAYLOAD_LEN - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_len += output_len;
    TEST_ASSERT_EQUAL(TEST_AES_PAYLOAD_LEN, total_len);

    memset(iv, 0xA5, sizeof(iv));
    iv[0] = 3;

    psa_cipher_abort(&operation);
    operation = (psa_cipher_operation_t)PSA_CIPHER_OPERATION_INIT;
    status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_CFB);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    status = psa_cipher_set_iv(&operation, iv, sizeof(iv));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_len = 0;
    status = psa_cipher_update(&operation, pkt->data, TEST_AES_PAYLOAD_LEN, pkt->data, TEST_AES_PAYLOAD_LEN, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_len += output_len;
    status = psa_cipher_finish(&operation, pkt->data + output_len, TEST_AES_PAYLOAD_LEN - output_len, &output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    total_len += output_len;
    TEST_ASSERT_EQUAL(TEST_AES_PAYLOAD_LEN, total_len);

    TEST_ASSERT_EQUAL_MEMORY(backup, pkt->data, TEST_AES_PAYLOAD_LEN);
    psa_cipher_abort(&operation);
    psa_destroy_key(key_id);
    free(backup);
    free(pkt);
}

TEST_CASE("mbedtls AES PSRAM ECC CFB128 in-place test", "[aes][psram_ecc_dma]")
{
    aes_psram_ecc_cfb128_inplace_test();
}
#endif // CONFIG_SPIRAM_ECC_ENABLE && SOC_AES_SUPPORT_DMA

void aes_psram_one_buf_ctr_test(void)
{
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t nonce[16];
    uint8_t key[16];
    psa_status_t status;
#if CONFIG_MBEDTLS_HARDWARE_AES
    size_t SZ = TEST_AES_CTR_DATA_LEN;
#else
    size_t SZ = TEST_AES_CTR_DATA_LEN - (TEST_AES_CTR_DATA_LEN % 16);
#endif
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
