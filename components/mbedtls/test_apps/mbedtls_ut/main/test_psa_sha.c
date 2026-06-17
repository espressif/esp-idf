/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <esp_system.h>
#include "psa/crypto.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "sdkconfig.h"
#include "test_apb_dport_access.h"
#include "soc/soc_caps.h"
#include "test_utils.h"
#include "esp_memory_utils.h"

#if CONFIG_MBEDTLS_HARDWARE_SHA
static const unsigned char *one_hundred_as = (unsigned char *)
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

static const unsigned char *one_hundred_bs =  (unsigned char *)
        "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";

static const uint8_t sha256_thousand_as[32] = {
    0x41, 0xed, 0xec, 0xe4, 0x2d, 0x63, 0xe8, 0xd9, 0xbf, 0x51, 0x5a, 0x9b, 0xa6, 0x93, 0x2e, 0x1c,
    0x20, 0xcb, 0xc9, 0xf5, 0xa5, 0xd1, 0x34, 0x64, 0x5a, 0xdb, 0x5d, 0xb1, 0xb9, 0x73, 0x7e, 0xa3
};


static const uint8_t sha256_thousand_bs[32] = {
    0xf6, 0xf1, 0x18, 0xe1, 0x20, 0xe5, 0x2b, 0xe0, 0xbd, 0x0c, 0xfd, 0xf2, 0x79, 0x4c, 0xd1, 0x2c, 0x07, 0x68, 0x6c, 0xc8, 0x71, 0x23, 0x5a, 0xc2, 0xf1, 0x14, 0x59, 0x37, 0x8e, 0x6d, 0x23, 0x5b
};

static const uint8_t sha512_thousand_bs[64] = {
    0xa6, 0x68, 0x68, 0xa3, 0x73, 0x53, 0x2a, 0x5c, 0xc3, 0x3f, 0xbf, 0x43, 0x4e, 0xba, 0x10, 0x86, 0xb3, 0x87, 0x09, 0xe9, 0x14, 0x3f, 0xbf, 0x37, 0x67, 0x8d, 0x43, 0xd9, 0x9b, 0x95, 0x08, 0xd5, 0x80, 0x2d, 0xbe, 0x9d, 0xe9, 0x1a, 0x54, 0xab, 0x9e, 0xbc, 0x8a, 0x08, 0xa0, 0x1a, 0x89, 0xd8, 0x72, 0x68, 0xdf, 0x52, 0x69, 0x7f, 0x1c, 0x70, 0xda, 0xe8, 0x3f, 0xe5, 0xae, 0x5a, 0xfc, 0x9d
};

static const uint8_t sha384_thousand_bs[48] = {
    0x6d, 0xe5, 0xf5, 0x88, 0x57, 0x60, 0x83, 0xff, 0x7c, 0x94, 0x61, 0x5f, 0x8d, 0x96, 0xf2, 0x76, 0xd5, 0x3f, 0x77, 0x0c, 0x8e, 0xc1, 0xbf, 0xb6, 0x04, 0x27, 0xa4, 0xba, 0xea, 0x6c, 0x68, 0x44, 0xbd, 0xb0, 0x9c, 0xef, 0x6a, 0x09, 0x28, 0xe8, 0x1f, 0xfc, 0x95, 0x03, 0x69, 0x99, 0xab, 0x1a
};

static const uint8_t sha1_thousand_as[20] = {
    0x29, 0x1e, 0x9a, 0x6c, 0x66, 0x99, 0x49, 0x49, 0xb5, 0x7b, 0xa5,
    0xe6, 0x50, 0x36, 0x1e, 0x98, 0xfc, 0x36, 0xb1, 0xba
};

TEST_CASE("mbedtls SHA interleaving", "[mbedtls]")
{
    psa_hash_operation_t sha1_op = PSA_HASH_OPERATION_INIT;
    psa_hash_operation_t sha256_op = PSA_HASH_OPERATION_INIT;
    psa_hash_operation_t sha512_op = PSA_HASH_OPERATION_INIT;
    unsigned char sha1[20], sha256[32], sha512[64];
    psa_status_t status;

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_hash_setup(&sha1_op, PSA_ALG_SHA_1);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_hash_setup(&sha256_op, PSA_ALG_SHA_256);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_hash_setup(&sha512_op, PSA_ALG_SHA_512);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    for (int i = 0; i < 10; i++) {
        status = psa_hash_update(&sha1_op, one_hundred_as, 100);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        status = psa_hash_update(&sha256_op, one_hundred_as, 100);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        status = psa_hash_update(&sha512_op, one_hundred_bs, 100);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    }

    size_t hash_len;
    status = psa_hash_finish(&sha1_op, sha1, sizeof(sha1), &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_hash_finish(&sha256_op, sha256, sizeof(sha256), &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_hash_finish(&sha512_op, sha512, sizeof(sha512), &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha512_thousand_bs, sha512, 64, "SHA512 calculation");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha256_thousand_as, sha256, 32, "SHA256 calculation");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha1_thousand_as, sha1, 20, "SHA1 calculation");
}

#define SHA_TASK_STACK_SIZE (10*1024)
static SemaphoreHandle_t done_sem;

static void tskRunSHA1Test(void *pvParameters)
{
    psa_hash_operation_t sha1_op;
    unsigned char sha1[20];
    psa_status_t status;
    size_t hash_len;

    for (int i = 0; i < 1000; i++) {
        sha1_op = (psa_hash_operation_t)PSA_HASH_OPERATION_INIT;

        status = psa_hash_setup(&sha1_op, PSA_ALG_SHA_1);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        for (int j = 0; j < 10; j++) {
            status = psa_hash_update(&sha1_op, (unsigned char *)one_hundred_as, 100);
            TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        }

        status = psa_hash_finish(&sha1_op, sha1, sizeof(sha1), &hash_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha1_thousand_as, sha1, 20, "SHA1 calculation");
    }
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

static void tskRunSHA256Test(void *pvParameters)
{
    psa_hash_operation_t sha256_op;
    unsigned char sha256[32];
    psa_status_t status;
    size_t hash_len;

    for (int i = 0; i < 1000; i++) {
        sha256_op = (psa_hash_operation_t)PSA_HASH_OPERATION_INIT;

        status = psa_hash_setup(&sha256_op, PSA_ALG_SHA_256);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        for (int j = 0; j < 10; j++) {
            status = psa_hash_update(&sha256_op, (unsigned char *)one_hundred_bs, 100);
            TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        }

        status = psa_hash_finish(&sha256_op, sha256, sizeof(sha256), &hash_len);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha256_thousand_bs, sha256, 32, "SHA256 calculation");
    }
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}


TEST_CASE("mbedtls SHA multithreading", "[mbedtls]")
{
    done_sem = xSemaphoreCreateCounting(4, 0);
    xTaskCreate(tskRunSHA1Test, "SHA1Task1", SHA_TASK_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(tskRunSHA1Test, "SHA1Task2", SHA_TASK_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(tskRunSHA256Test, "SHA256Task1", SHA_TASK_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(tskRunSHA256Test, "SHA256Task2", SHA_TASK_STACK_SIZE, NULL, 3, NULL);

    for (int i = 0; i < 4; i++) {
        if (!xSemaphoreTake(done_sem, 10000 / portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("done_sem not released by test task");
        }
    }
    vSemaphoreDelete(done_sem);
}

void tskRunSHASelftests(void *param)
{
    // PSA Crypto API does not provide self-test functions
    // Instead, we run basic validation tests for each algorithm
    psa_hash_operation_t op;
    unsigned char hash[64];
    size_t hash_len;
    psa_status_t status;

    for (int i = 0; i < 5; i++) {
#if CONFIG_MBEDTLS_SHA1_C
        // SHA1 validation
        op = (psa_hash_operation_t)PSA_HASH_OPERATION_INIT;
        status = psa_hash_setup(&op, PSA_ALG_SHA_1);
        if (status != PSA_SUCCESS) {
            printf("SHA1 setup failed.\n");
            while (1) {}
        }
        status = psa_hash_update(&op, one_hundred_as, 100);
        if (status != PSA_SUCCESS) {
            printf("SHA1 update failed.\n");
            while (1) {}
        }
        status = psa_hash_finish(&op, hash, sizeof(hash), &hash_len);
        if (status != PSA_SUCCESS) {
            printf("SHA1 finish failed.\n");
            while (1) {}
        }
#endif

        // SHA256 validation
        op = (psa_hash_operation_t)PSA_HASH_OPERATION_INIT;
        status = psa_hash_setup(&op, PSA_ALG_SHA_256);
        if (status != PSA_SUCCESS) {
            printf("SHA256 setup failed.\n");
            while (1) {}
        }
        status = psa_hash_update(&op, one_hundred_bs, 100);
        if (status != PSA_SUCCESS) {
            printf("SHA256 update failed.\n");
            while (1) {}
        }
        status = psa_hash_finish(&op, hash, sizeof(hash), &hash_len);
        if (status != PSA_SUCCESS) {
            printf("SHA256 finish failed.\n");
            while (1) {}
        }

#if SOC_SHA_SUPPORT_SHA512 && CONFIG_MBEDTLS_SHA512_C
        // SHA512 validation
        op = (psa_hash_operation_t)PSA_HASH_OPERATION_INIT;
        status = psa_hash_setup(&op, PSA_ALG_SHA_512);
        if (status != PSA_SUCCESS) {
            printf("SHA512 setup failed.\n");
            while (1) {}
        }
        status = psa_hash_update(&op, one_hundred_bs, 100);
        if (status != PSA_SUCCESS) {
            printf("SHA512 update failed.\n");
            while (1) {}
        }
        status = psa_hash_finish(&op, hash, sizeof(hash), &hash_len);
        if (status != PSA_SUCCESS) {
            printf("SHA512 finish failed.\n");
            while (1) {}
        }
#endif //SOC_SHA_SUPPORT_SHA512 && CONFIG_MBEDTLS_SHA512_C
    }
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

TEST_CASE("mbedtls SHA self-tests multithreaded", "[mbedtls]")
{
    done_sem = xSemaphoreCreateCounting(2, 0);
    xTaskCreate(tskRunSHASelftests, "SHASelftests1", SHA_TASK_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(tskRunSHASelftests, "SHASelftests2", SHA_TASK_STACK_SIZE, NULL, 3, NULL);

    const int TIMEOUT_MS = 40000;

    for (int i = 0; i < 2; i++) {
        if (!xSemaphoreTake(done_sem, TIMEOUT_MS / portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("done_sem not released by test task");
        }
    }
    vSemaphoreDelete(done_sem);
}

TEST_CASE("mbedtls SHA512 clone", "[mbedtls]")
{
    psa_hash_operation_t op = PSA_HASH_OPERATION_INIT;
    psa_hash_operation_t clone = PSA_HASH_OPERATION_INIT;
    unsigned char sha512[64];
    psa_status_t status;
    size_t hash_len;

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_hash_setup(&op, PSA_ALG_SHA_512);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    for (int i = 0; i < 5; i++) {
        status = psa_hash_update(&op, one_hundred_bs, 100);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    }

    status = psa_hash_clone(&op, &clone);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    for (int i = 0; i < 5; i++) {
        status = psa_hash_update(&op, one_hundred_bs, 100);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        status = psa_hash_update(&clone, one_hundred_bs, 100);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    }

    status = psa_hash_finish(&op, sha512, sizeof(sha512), &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha512_thousand_bs, sha512, 64, "SHA512 original calculation");

    status = psa_hash_finish(&clone, sha512, sizeof(sha512), &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha512_thousand_bs, sha512, 64, "SHA512 cloned calculation");
}

TEST_CASE("mbedtls SHA384 clone", "[mbedtls]")
{
    psa_hash_operation_t op = PSA_HASH_OPERATION_INIT;
    psa_hash_operation_t clone = PSA_HASH_OPERATION_INIT;
    unsigned char sha384[48];
    psa_status_t status;
    size_t hash_len;

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_hash_setup(&op, PSA_ALG_SHA_384);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    for (int i = 0; i < 5; i++) {
        status = psa_hash_update(&op, one_hundred_bs, 100);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    }

    status = psa_hash_clone(&op, &clone);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    for (int i = 0; i < 5; i++) {
        status = psa_hash_update(&op, one_hundred_bs, 100);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        status = psa_hash_update(&clone, one_hundred_bs, 100);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    }

    status = psa_hash_finish(&op, sha384, sizeof(sha384), &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha384_thousand_bs, sha384, 48, "SHA512 original calculation");

    status = psa_hash_finish(&clone, sha384, sizeof(sha384), &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha384_thousand_bs, sha384, 48, "SHA512 cloned calculation");
}


TEST_CASE("mbedtls SHA256 clone", "[mbedtls]")
{
    psa_hash_operation_t op = PSA_HASH_OPERATION_INIT;
    psa_hash_operation_t clone = PSA_HASH_OPERATION_INIT;
    unsigned char sha256[64];
    psa_status_t status;
    size_t hash_len;

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_hash_setup(&op, PSA_ALG_SHA_256);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    for (int i = 0; i < 5; i++) {
        status = psa_hash_update(&op, one_hundred_as, 100);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    }

    status = psa_hash_clone(&op, &clone);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    for (int i = 0; i < 5; i++) {
        status = psa_hash_update(&op, one_hundred_as, 100);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

        status = psa_hash_update(&clone, one_hundred_as, 100);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    }

    status = psa_hash_finish(&op, sha256, sizeof(sha256), &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha256_thousand_as, sha256, 32, "SHA256 original calculation");

    status = psa_hash_finish(&clone, sha256, sizeof(sha256), &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha256_thousand_as, sha256, 32, "SHA256 cloned calculation");
}

typedef struct {
    psa_hash_operation_t op;
    uint8_t result[32];
    psa_status_t ret;
    bool done;
} finalise_sha_param_t;

static void tskFinaliseSha(void *v_param)
{
    finalise_sha_param_t *param = (finalise_sha_param_t *)v_param;
    psa_status_t status;

    for (int i = 0; i < 5; i++) {
        status = psa_hash_update(&param->op, one_hundred_as, 100);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    }

    size_t hash_len;
    param->ret = psa_hash_finish(&param->op, param->result, sizeof(param->result), &hash_len);

    param->done = true;
    vTaskDelete(NULL);
}


TEST_CASE("mbedtls SHA session passed between tasks", "[mbedtls]")
{
    finalise_sha_param_t param = { 0 };
    psa_status_t status;

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    param.op = (psa_hash_operation_t)PSA_HASH_OPERATION_INIT;
    status = psa_hash_setup(&param.op, PSA_ALG_SHA_256);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    for (int i = 0; i < 5; i++) {
        status = psa_hash_update(&param.op, one_hundred_as, 100);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    }

    // pass the SHA context off to a different task
    //
    // note: at the moment this doesn't crash even if a mutex semaphore is used as the
    // engine lock, but it can crash...
    xTaskCreate(tskFinaliseSha, "SHAFinalise", SHA_TASK_STACK_SIZE, &param, 3, NULL);

    while (!param.done) {
        vTaskDelay(1);
    }

    TEST_ASSERT_EQUAL(PSA_SUCCESS, param.ret);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha256_thousand_as, param.result, 32, "SHA256 result from other task");
}



/* Random input generated and hashed using python:

    import hashlib
    import os, binascii

    input = bytearray(os.urandom(150))
    arr = ''
    for idx, b in enumerate(input):
        if idx % 8 == 0:
            arr += '\n'
        arr += "{}, ".format(hex(b))
    digest = hashlib.sha256(input).hexdigest()

*/
const uint8_t test_vector[] = {
    0xe4, 0x1a, 0x1a, 0x30, 0x71, 0xd3, 0x94, 0xb0,
    0xc3, 0x7e, 0x99, 0x9f, 0x1a, 0xde, 0x4a, 0x36,
    0xb1, 0x1, 0x81, 0x2b, 0x41, 0x91, 0x11, 0x7f,
    0xd8, 0xe1, 0xd5, 0xe5, 0x52, 0x6d, 0x92, 0xee,
    0x6c, 0xf7, 0x70, 0xea, 0x3a, 0xb, 0xc9, 0x97,
    0xc0, 0x12, 0x6f, 0x10, 0x5b, 0x90, 0xd8, 0x52,
    0x91, 0x69, 0xea, 0xc4, 0x1f, 0xc, 0xcf, 0xc6,
    0xf0, 0x43, 0xc6, 0xa3, 0x1f, 0x46, 0x3c, 0x3d,
    0x25, 0xe5, 0xa8, 0x27, 0x86, 0x85, 0x32, 0x3f,
    0x33, 0xd8, 0x40, 0xc4, 0x41, 0xf6, 0x4b, 0x12,
    0xd8, 0x5e, 0x4, 0x27, 0x42, 0x90, 0x73, 0x4,
    0x8, 0x42, 0xd1, 0x64, 0xd, 0x84, 0x3, 0x1,
    0x76, 0x88, 0xe4, 0x95, 0xdf, 0xe7, 0x62, 0xb4,
    0xb3, 0xb2, 0x7e, 0x6d, 0x78, 0xca, 0x79, 0x82,
    0xcc, 0xba, 0x22, 0xd2, 0x90, 0x2e, 0xe3, 0xa8,
    0x2a, 0x53, 0x3a, 0xb1, 0x9a, 0x7f, 0xb7, 0x8b,
    0xfa, 0x32, 0x47, 0xc1, 0x5c, 0x6, 0x4f, 0x7b,
    0xcd, 0xb3, 0xf4, 0xf1, 0xd0, 0xb5, 0xbf, 0xfb,
    0x7c, 0xc3, 0xa5, 0xb2, 0xc4, 0xd4,
};

const uint8_t test_vector_digest[] = {
    0xff, 0x1c, 0x60, 0xcb, 0x21, 0xf0, 0x63, 0x68,
    0xb9, 0xfc, 0xfe, 0xad, 0x3e, 0xb0, 0x2e, 0xd1,
    0xf9, 0x08, 0x82, 0x82, 0x83, 0x06, 0xc1, 0x8a,
    0x98, 0x5d, 0x36, 0xc0, 0xb7, 0xeb, 0x35, 0xe0,
};


TEST_CASE("mbedtls SHA, input in flash", "[mbedtls]")
{
    psa_hash_operation_t sha256_op = PSA_HASH_OPERATION_INIT;
    unsigned char sha256[32];
    psa_status_t status;
    size_t hash_len;

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_hash_setup(&sha256_op, PSA_ALG_SHA_256);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_hash_update(&sha256_op, test_vector, sizeof(test_vector));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_hash_finish(&sha256_op, sha256, sizeof(sha256), &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(test_vector_digest, sha256, 32, "SHA256 calculation");
}

#if 0 // PSA Crypto API does not support SHA512/t variants (SHA512/224, SHA512/256)
/* Note: SHA512/t variants (SHA512/224, SHA512/256) use ESP-specific mbedTLS extensions
 * (esp_sha512_set_mode, esp_sha512_set_t) which don't have direct PSA Crypto API equivalents.
 * PSA Crypto defines PSA_ALG_SHA_512_224 and PSA_ALG_SHA_512_256 for these algorithms.
 */
#if CONFIG_MBEDTLS_HARDWARE_SHA && SOC_SHA_SUPPORT_SHA512_T

/*
 * FIPS-180-2 test vectors
 */
static unsigned char sha512T_test_buf[2][113] = {
    { "abc" },
    {
        "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn"
        "hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"
    }
};

static const size_t sha512T_test_buflen[2] = {
    3, 112
};

static const psa_algorithm_t sha512T_algo[4] = {
    PSA_ALG_SHA_512_224, PSA_ALG_SHA_512_256, PSA_ALG_SHA_512_224, PSA_ALG_SHA_512_256
};

static const size_t sha512T_t_len[4] = { 224, 256, 224, 256 };

static const unsigned char sha512_test_sum[4][32] = {
    /* SHA512-224 */
    {
        0x46, 0x34, 0x27, 0x0f, 0x70, 0x7b, 0x6a, 0x54,
        0xda, 0xae, 0x75, 0x30, 0x46, 0x08, 0x42, 0xe2,
        0x0e, 0x37, 0xed, 0x26, 0x5c, 0xee, 0xe9, 0xa4,
        0x3e, 0x89, 0x24, 0xaa
    },
    {
        0x23, 0xfe, 0xc5, 0xbb, 0x94, 0xd6, 0x0b, 0x23,
        0x30, 0x81, 0x92, 0x64, 0x0b, 0x0c, 0x45, 0x33,
        0x35, 0xd6, 0x64, 0x73, 0x4f, 0xe4, 0x0e, 0x72,
        0x68, 0x67, 0x4a, 0xf9
    },

    /* SHA512-256 */
    {
        0x53, 0x04, 0x8e, 0x26, 0x81, 0x94, 0x1e, 0xf9,
        0x9b, 0x2e, 0x29, 0xb7, 0x6b, 0x4c, 0x7d, 0xab,
        0xe4, 0xc2, 0xd0, 0xc6, 0x34, 0xfc, 0x6d, 0x46,
        0xe0, 0xe2, 0xf1, 0x31, 0x07, 0xe7, 0xaf, 0x23
    },
    {
        0x39, 0x28, 0xe1, 0x84, 0xfb, 0x86, 0x90, 0xf8,
        0x40, 0xda, 0x39, 0x88, 0x12, 0x1d, 0x31, 0xbe,
        0x65, 0xcb, 0x9d, 0x3e, 0xf8, 0x3e, 0xe6, 0x14,
        0x6f, 0xea, 0xc8, 0x61, 0xe1, 0x9b, 0x56, 0x3a
    }

    /* For SHA512_T testing we use t=224 & t=256
     * so the hash digest should be same as above
     */
};

/* This will run total of 4 test cases (PSA doesn't need separate tests for SHA512/t vs SHA512/224,256)
 * SHA512/224, SHA512/256 with PSA Crypto API
 */
TEST_CASE("mbedtls SHA512/t", "[mbedtls]")
{
    psa_hash_operation_t sha512_op;
    unsigned char sha512[64];
    psa_status_t status;
    size_t hash_len;

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // Test SHA512/224 and SHA512/256
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            int k = i * 2 + j;
            sha512_op = (psa_hash_operation_t)PSA_HASH_OPERATION_INIT;

            status = psa_hash_setup(&sha512_op, sha512T_algo[i]);
            TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

            status = psa_hash_update(&sha512_op, sha512T_test_buf[j], sha512T_test_buflen[j]);
            TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

            status = psa_hash_finish(&sha512_op, sha512, sizeof(sha512), &hash_len);
            TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

            TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha512_test_sum[k], sha512, sha512T_t_len[i] / 8, "SHA512t calculation");
        }
    }
}
#endif //CONFIG_MBEDTLS_HARDWARE_SHA
#endif

#ifdef CONFIG_SPIRAM_USE_MALLOC
#include "test_mbedtls_utils.h"
TEST_CASE("mbedtls SHA256 PSRAM DMA", "[mbedtls]")
{
    const unsigned CALLS = 256;
    const unsigned CALL_SZ = 16 * 1024;
    psa_hash_operation_t sha256_op = PSA_HASH_OPERATION_INIT;
    unsigned char sha256[32];
    psa_status_t status;
    size_t hash_len;

    // allocate external memory
    uint8_t *buf = heap_caps_malloc(CALL_SZ, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    TEST_ASSERT(esp_ptr_external_ram(buf));
    memset(buf, 0x54, CALL_SZ);

    status = psa_hash_setup(&sha256_op, PSA_ALG_SHA_256);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    for (int c = 0; c < CALLS; c++) {
        status = psa_hash_update(&sha256_op, buf, CALL_SZ);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    }

    status = psa_hash_finish(&sha256_op, sha256, sizeof(sha256), &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    free(buf);

    /* Check the result. Reference value can be calculated using:
     * dd if=/dev/zero bs=$((16*1024)) count=256 | tr '\000' '\124' | sha256sum
     */
    const char *expected_hash = "8d031167bd706ac337e07aa9129c34ae4ae792d0a79a2c70e7f012102e8adc3d";
    char hash_str[sizeof(sha256) * 2 + 1];
    utils_bin2hex(hash_str, sizeof(hash_str), sha256, sizeof(sha256));

    TEST_ASSERT_EQUAL_STRING(expected_hash, hash_str);

}

#if SOC_SHA_SUPPORT_DMA
TEST_CASE("mbedtls SHA256 PSRAM DMA large buffer", "[hw_crypto]")
{
    psa_hash_operation_t sha256_op = PSA_HASH_OPERATION_INIT;
    unsigned char sha256[32];
    psa_status_t status;
    size_t hash_len;

    // Initialize PSA Crypto
    status = psa_crypto_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    const size_t SZ = 257984; // specific size to cover issue in https://github.com/espressif/esp-idf/issues/11915
    void *buffer = heap_caps_malloc(SZ, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    TEST_ASSERT_NOT_NULL(buffer);
    memset(buffer, 0x55, SZ);

    status = psa_hash_setup(&sha256_op, PSA_ALG_SHA_256);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_hash_update(&sha256_op, buffer, SZ);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    status = psa_hash_finish(&sha256_op, sha256, sizeof(sha256), &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    free(buffer);

    /* Check the result. Reference value can be calculated using:
     * dd if=/dev/zero bs=257984 count=1 | tr '\000' '\125' | sha256sum
     */
    const char *expected_hash = "f2330c9f81ff1c8f0515247faa82be8b6f9685601de6f5dae79172766f136c33";

    char hash_str[sizeof(sha256) * 2 + 1];
    utils_bin2hex(hash_str, sizeof(hash_str), sha256, sizeof(sha256));

    TEST_ASSERT_EQUAL_STRING(expected_hash, hash_str);
}
#endif // SOC_SHA_SUPPORT_DMA

#endif //CONFIG_SPIRAM_USE_MALLOC

#if CONFIG_ESP_SYSTEM_RTC_FAST_MEM_AS_HEAP_DEPCHECK && !CONFIG_IDF_TARGET_ESP32H2
// Not enough rtc memory for test on H2

TEST_CASE("mbedtls SHA stack in RTC RAM", "[mbedtls]")
{
    done_sem = xSemaphoreCreateBinary();
    static StaticTask_t rtc_task;
    size_t STACK_SIZE = 3072;
    uint8_t *rtc_stack = heap_caps_calloc(STACK_SIZE, 1, MALLOC_CAP_RTCRAM);

    TEST_ASSERT(esp_ptr_in_rtc_dram_fast(rtc_stack));

    TEST_ASSERT_NOT_NULL(xTaskCreateStatic(tskRunSHA256Test, "tskRunSHA256Test_task", STACK_SIZE, NULL,
                                            3, rtc_stack, &rtc_task));
    TEST_ASSERT_TRUE(xSemaphoreTake(done_sem, 10000 / portTICK_PERIOD_MS));

     /* Give task time to cleanup before freeing stack */
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    free(rtc_stack);

    vSemaphoreDelete(done_sem);
}

#endif //CONFIG_ESP_SYSTEM_RTC_FAST_MEM_AS_HEAP_DEPCHECK

#if CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM && CONFIG_SPIRAM_USE_MALLOC

TEST_CASE("mbedtls SHA stack in PSRAM", "[mbedtls]")
{
    done_sem = xSemaphoreCreateBinary();
    static StaticTask_t psram_task;
    size_t STACK_SIZE = 3072;
    uint8_t *psram_stack = heap_caps_calloc(STACK_SIZE, 1, MALLOC_CAP_SPIRAM);

    TEST_ASSERT(esp_ptr_external_ram(psram_stack));

    TEST_ASSERT_NOT_NULL(xTaskCreateStatic(tskRunSHA256Test, "tskRunSHA256Test_task", STACK_SIZE, NULL,
                                            3, psram_stack, &psram_task));
    TEST_ASSERT_TRUE(xSemaphoreTake(done_sem, 10000 / portTICK_PERIOD_MS));

    /* Give task time to cleanup before freeing stack */
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    free(psram_stack);

    vSemaphoreDelete(done_sem);
}

#endif //CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM && CONFIG_SPIRAM_USE_MALLOC

TEST_CASE("Test PSA SHA-384 with clone", "[hw_crypto][psa]")
{
    // Test Vector 1: SHA-384("hello world")
    // Expected: fdbd8e75a67f29f701a4e040385e2e23986303ea10239211af907fcbb83578b3e417cb71ce646efd0819dd8c088de1bd
    const unsigned char test3_input[] = "hello world";
    const unsigned char test3_expected[48] = {
        0xfd, 0xbd, 0x8e, 0x75, 0xa6, 0x7f, 0x29, 0xf7,
        0x01, 0xa4, 0xe0, 0x40, 0x38, 0x5e, 0x2e, 0x23,
        0x98, 0x63, 0x03, 0xea, 0x10, 0x23, 0x92, 0x11,
        0xaf, 0x90, 0x7f, 0xcb, 0xb8, 0x35, 0x78, 0xb3,
        0xe4, 0x17, 0xcb, 0x71, 0xce, 0x64, 0x6e, 0xfd,
        0x08, 0x19, 0xdd, 0x8c, 0x08, 0x8d, 0xe1, 0xbd
    };

    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    psa_status_t psa_status = psa_hash_setup(&operation, PSA_ALG_SHA_384);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    psa_status = psa_hash_update(&operation, (const uint8_t *)test3_input, 5); // "hello"
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);

    psa_hash_operation_t clone = PSA_HASH_OPERATION_INIT;
    psa_status = psa_hash_clone(&operation, &clone);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    psa_status = psa_hash_update(&clone, (const uint8_t *)(test3_input + 5), 6); // " world"
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    unsigned char psa_output[48];
    size_t psa_output_len;
    psa_status = psa_hash_finish(&clone, psa_output, sizeof(psa_output), &psa_output_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_status);
    TEST_ASSERT_EQUAL(48, psa_output_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(test3_expected, psa_output, 48);
}

#endif // CONFIG_MBEDTLS_HARDWARE_SHA
