/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* mbedTLS AES performance test
*/
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <esp_system.h>
#define MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS
#include "psa/crypto.h"
#include "unity.h"
#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "test_utils.h"
#include "ccomp_timer.h"
#include "crypto_performance.h"

#if CONFIG_MBEDTLS_HARDWARE_AES
TEST_CASE("mbedtls AES performance", "[aes][timeout=60]")
{
    const unsigned CALLS = 256;
    const unsigned CALL_SZ = 32 * 1024;
    float elapsed_usec;
    uint8_t iv[16];
    uint8_t key[16];

    psa_status_t status;

    memset(iv, 0xEE, 16);
    memset(key, 0x44, 16);

    // allocate internal memory
    uint8_t *buf = heap_caps_malloc(CALL_SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buf);
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CBC_NO_PADDING);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);
    status = psa_import_key(&attributes, key, sizeof(key), &key_id);
    if (status != PSA_SUCCESS) {
        TEST_FAIL_MESSAGE("Failed to import key");
    }

    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_CBC_NO_PADDING);
    if (status != PSA_SUCCESS) {
        printf("Failed to setup AES encryption with status: %ld\n", status);
        TEST_FAIL_MESSAGE("Failed to setup AES encryption");
    }

    status = psa_cipher_set_iv(&operation, iv, sizeof(iv));
    if (status != PSA_SUCCESS) {
        TEST_FAIL_MESSAGE("Failed to set IV for AES encryption");
    }

    ccomp_timer_start();
    size_t output_length = 0;
    for (int c = 0; c < CALLS; c++) {
        memset(buf, 0xAA, CALL_SZ);
        psa_cipher_update(&operation, buf, CALL_SZ, buf, CALL_SZ, &output_length);
    }
    psa_cipher_finish(&operation, buf + output_length, CALL_SZ - output_length, &output_length);
    elapsed_usec = ccomp_timer_stop();

    /* Sanity check: make sure the last ciphertext block matches
       what we expect to see.

       Last block produced via this Python:
       import os, binascii
       from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
       from cryptography.hazmat.backends import default_backend
       key = b'\x44' * 16
       iv = b'\xee' * 16
       cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=default_backend())
       encryptor = cipher.encryptor()
       ct = encryptor.update(b'\xaa' * 256 * 32 * 1024) + encryptor.finalize()
       print(binascii.hexlify(ct[-16:]))
    */
    const uint8_t expected_last_block[] = {
        0x50, 0x81, 0xe0, 0xe1, 0x15, 0x2f, 0x14, 0xe9,
        0x97, 0xa0, 0xc6, 0xe6, 0x36, 0xf3, 0x5c, 0x25,
    };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_last_block, buf + CALL_SZ - 16, 16);

    psa_destroy_key(key_id);
    psa_reset_key_attributes(&attributes);
    free(buf);

    // bytes/usec = MB/sec
    float mb_sec = (CALL_SZ * CALLS) / elapsed_usec;
    printf("Encryption rate %.3fMB/sec\n", mb_sec);
    // Don't put a hard limit on software AES performance
    TEST_PERFORMANCE_CCOMP_GREATER_THAN(AES_CBC_THROUGHPUT_MBSEC, "%.3fMB/sec", mb_sec);
}
#endif
