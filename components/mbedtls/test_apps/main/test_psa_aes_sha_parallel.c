/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdbool.h>
#include <esp_system.h>
#include "unity.h"
#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "test_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "psa/crypto.h"

#if CONFIG_MBEDTLS_HARDWARE_AES || CONFIG_MBEDTLS_HARDWARE_SHA

static SemaphoreHandle_t done_sem;

static const unsigned char *one_hundred_bs =  (unsigned char *)
        "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";

static const uint8_t sha256_thousand_bs[32] = {
    0xf6, 0xf1, 0x18, 0xe1, 0x20, 0xe5, 0x2b, 0xe0, 0xbd, 0x0c, 0xfd, 0xf2, 0x79, 0x4c, 0xd1, 0x2c, 0x07, 0x68, 0x6c, 0xc8, 0x71, 0x23, 0x5a, 0xc2, 0xf1, 0x14, 0x59, 0x37, 0x8e, 0x6d, 0x23, 0x5b
};

static void tskRunSHA256Test(void *pvParameters)
{
    unsigned char sha256[32];
    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    psa_status_t status;
    size_t hash_length = 0;
    for (int i = 0; i < 1000; i++) {
        status = psa_hash_setup(&operation, PSA_ALG_SHA_256);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        for (int j = 0; j < 10; j++) {
            status = psa_hash_update(&operation, (unsigned char *)one_hundred_bs, 100);
            TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        }
        status = psa_hash_finish(&operation, sha256, sizeof(sha256), &hash_length);
        operation = psa_hash_operation_init();
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha256_thousand_bs, sha256, 32, "SHA256 calculation");
    }
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}


static void tskRunAES256Test(void *pvParameters)
{
    static const uint8_t iv[] = {
        0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09,
        0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,
    };

    static const uint8_t key_256[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    };

    psa_key_id_t key_id;
    psa_algorithm_t alg = PSA_ALG_CBC_NO_PADDING;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, sizeof(key_256) * 8);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_import_key(&attributes, key_256, sizeof(key_256), &key_id));
    psa_reset_key_attributes(&attributes);

    for (int i = 0; i <1000; i++)
    {
        const unsigned SZ = 1600;
        psa_cipher_operation_t ctx = PSA_CIPHER_OPERATION_INIT;
        uint8_t nonce[16];

        const uint8_t expected_cipher_end[] = {
            0x3e, 0x68, 0x8a, 0x02, 0xe6, 0xf2, 0x6a, 0x9e,
            0x9b, 0xb2, 0xc0, 0xc4, 0x63, 0x63, 0xd9, 0x25,
            0x51, 0xdc, 0xc2, 0x71, 0x96, 0xb3, 0xe5, 0xcd,
            0xbd, 0x0e, 0xf2, 0xef, 0xa9, 0xab, 0xab, 0x2d,
        };

        memcpy(nonce, iv, 16);

        // allocate internal memory
        uint8_t *ciphertext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
        uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
        uint8_t *decryptedtext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);

        TEST_ASSERT_NOT_NULL(ciphertext);
        TEST_ASSERT_NOT_NULL(plaintext);
        TEST_ASSERT_NOT_NULL(decryptedtext);

        psa_cipher_encrypt_setup(&ctx, key_id, PSA_ALG_CBC_NO_PADDING);
        psa_cipher_set_iv(&ctx, nonce, sizeof(nonce));

        memset(plaintext, 0x3A, SZ);
        memset(decryptedtext, 0x0, SZ);

        // Encrypt
        size_t enc_len = 0;
        psa_cipher_update(&ctx, plaintext, SZ, ciphertext, SZ, &enc_len);
        psa_cipher_finish(&ctx, ciphertext + enc_len, SZ - enc_len, &enc_len);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_end, ciphertext + SZ - 32, 32);

        // Decrypt
        memcpy(nonce, iv, 16);
        psa_cipher_decrypt_setup(&ctx, key_id, PSA_ALG_CBC_NO_PADDING);
        psa_cipher_set_iv(&ctx, nonce, sizeof(nonce));
        psa_cipher_update(&ctx, ciphertext, SZ, decryptedtext, SZ, &enc_len);
        psa_cipher_finish(&ctx, decryptedtext + enc_len, SZ - enc_len, &enc_len);

        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

        psa_cipher_abort(&ctx);
        free(plaintext);
        free(ciphertext);
        free(decryptedtext);
    }
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);

}

#define TASK_STACK_SIZE (20*1024)

TEST_CASE("mbedtls AES/SHA multithreading", "[mbedtls]")
{
    done_sem = xSemaphoreCreateCounting(2, 0);

    xTaskCreate(tskRunSHA256Test, "SHA256Task", TASK_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(tskRunAES256Test, "AES256Task", TASK_STACK_SIZE, NULL, 3, NULL);

    for (int i = 0; i < 2; i++) {
        if (!xSemaphoreTake(done_sem, 10000 / portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("done_sem not released by test task");
        }
    }

    vSemaphoreDelete(done_sem);
}
#endif // CONFIG_MBEDTLS_HARDWARE_AES || CONFIG_MBEDTLS_HARDWARE_SHA
