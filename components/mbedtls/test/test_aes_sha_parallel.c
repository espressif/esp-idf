/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdbool.h>
#include <esp_system.h>
#include "mbedtls/aes.h"
#include "mbedtls/sha256.h"
#include "unity.h"
#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "test_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"


static SemaphoreHandle_t done_sem;

static const unsigned char *one_hundred_bs =  (unsigned char *)
        "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";

static const uint8_t sha256_thousand_bs[32] = {
    0xf6, 0xf1, 0x18, 0xe1, 0x20, 0xe5, 0x2b, 0xe0, 0xbd, 0x0c, 0xfd, 0xf2, 0x79, 0x4c, 0xd1, 0x2c, 0x07, 0x68, 0x6c, 0xc8, 0x71, 0x23, 0x5a, 0xc2, 0xf1, 0x14, 0x59, 0x37, 0x8e, 0x6d, 0x23, 0x5b
};

static void tskRunSHA256Test(void *pvParameters)
{
    mbedtls_sha256_context sha256_ctx;
    unsigned char sha256[32];

    for (int i = 0; i < 1000; i++) {

        mbedtls_sha256_init(&sha256_ctx);
        TEST_ASSERT_EQUAL(0, mbedtls_sha256_starts(&sha256_ctx, false));
        for (int j = 0; j < 10; j++) {
            TEST_ASSERT_EQUAL(0, mbedtls_sha256_update(&sha256_ctx, (unsigned char *)one_hundred_bs, 100));
        }
        TEST_ASSERT_EQUAL(0, mbedtls_sha256_finish(&sha256_ctx, sha256));
        mbedtls_sha256_free(&sha256_ctx);
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

    for (int i = 0; i <1000; i++)
    {
        const unsigned SZ = 1600;
        mbedtls_aes_context ctx;
        uint8_t nonce[16];

        const uint8_t expected_cipher_end[] = {
            0x3e, 0x68, 0x8a, 0x02, 0xe6, 0xf2, 0x6a, 0x9e,
            0x9b, 0xb2, 0xc0, 0xc4, 0x63, 0x63, 0xd9, 0x25,
            0x51, 0xdc, 0xc2, 0x71, 0x96, 0xb3, 0xe5, 0xcd,
            0xbd, 0x0e, 0xf2, 0xef, 0xa9, 0xab, 0xab, 0x2d,
        };

        memcpy(nonce, iv, 16);

        // allocate internal memory
        uint8_t *chipertext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
        uint8_t *plaintext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
        uint8_t *decryptedtext = heap_caps_malloc(SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);

        TEST_ASSERT_NOT_NULL(chipertext);
        TEST_ASSERT_NOT_NULL(plaintext);
        TEST_ASSERT_NOT_NULL(decryptedtext);

        mbedtls_aes_init(&ctx);
        mbedtls_aes_setkey_enc(&ctx, key_256, 256);

        memset(plaintext, 0x3A, SZ);
        memset(decryptedtext, 0x0, SZ);

        // Encrypt
        mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, SZ, nonce, plaintext, chipertext);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_cipher_end, chipertext + SZ - 32, 32);

        // Decrypt
        memcpy(nonce, iv, 16);
        mbedtls_aes_setkey_dec(&ctx, key_256, 256);
        mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, SZ, nonce, chipertext, decryptedtext);

        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decryptedtext, SZ);

        mbedtls_aes_free(&ctx);
        free(plaintext);
        free(chipertext);
        free(decryptedtext);
    }
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);

}

#include "esp_crypto_shared_gdma.h"

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
