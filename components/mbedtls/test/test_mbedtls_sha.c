/* mbedTLS SHA unit tests
 */

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <esp_system.h>
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "sdkconfig.h"
#include "test_apb_dport_access.h"

TEST_CASE("mbedtls SHA self-tests", "[mbedtls]")
{
    start_apb_access_loop();
    TEST_ASSERT_FALSE_MESSAGE(mbedtls_sha1_self_test(1), "SHA1 self-tests should pass.");
    TEST_ASSERT_FALSE_MESSAGE(mbedtls_sha256_self_test(1), "SHA256 self-tests should pass.");
    TEST_ASSERT_FALSE_MESSAGE(mbedtls_sha512_self_test(1), "SHA512 self-tests should pass.");
    TEST_ASSERT_FALSE_MESSAGE(mbedtls_sha512_self_test(1), "SHA512 self-tests should pass.");
    verify_apb_access_loop();
}

static const unsigned char *one_hundred_as = (unsigned char *)
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

static const unsigned char *one_hundred_bs =  (unsigned char *)
    "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";

static const uint8_t sha256_thousand_as[32] = {
    0x41, 0xed, 0xec, 0xe4, 0x2d, 0x63, 0xe8, 0xd9, 0xbf, 0x51, 0x5a, 0x9b, 0xa6, 0x93, 0x2e, 0x1c,
    0x20, 0xcb, 0xc9, 0xf5, 0xa5, 0xd1, 0x34, 0x64, 0x5a, 0xdb, 0x5d, 0xb1, 0xb9, 0x73, 0x7e, 0xa3 };

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
    0xe6, 0x50, 0x36, 0x1e, 0x98, 0xfc, 0x36, 0xb1, 0xba };

TEST_CASE("mbedtls SHA interleaving", "[mbedtls]")
{
    mbedtls_sha1_context sha1_ctx;
    mbedtls_sha256_context sha256_ctx;
    mbedtls_sha512_context sha512_ctx;
    unsigned char sha1[20], sha256[32], sha512[64];

    mbedtls_sha1_init(&sha1_ctx);
    mbedtls_sha256_init(&sha256_ctx);
    mbedtls_sha512_init(&sha512_ctx);

    mbedtls_sha1_starts(&sha1_ctx);
    mbedtls_sha256_starts(&sha256_ctx, false);
    mbedtls_sha512_starts(&sha512_ctx, false);

    for (int i = 0; i < 10; i++) {
        mbedtls_sha1_update(&sha1_ctx, one_hundred_as, 100);
        mbedtls_sha256_update(&sha256_ctx, one_hundred_as, 100);
        mbedtls_sha512_update(&sha512_ctx, one_hundred_bs, 100);
    }

    mbedtls_sha1_finish(&sha1_ctx, sha1);
    mbedtls_sha256_finish(&sha256_ctx, sha256);
    mbedtls_sha512_finish(&sha512_ctx, sha512);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha512_thousand_bs, sha512, 64, "SHA512 calculation");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha256_thousand_as, sha256, 32, "SHA256 calculation");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha1_thousand_as, sha1, 20, "SHA1 calculation");
}

static xSemaphoreHandle done_sem;
static void tskRunSHA1Test(void *pvParameters)
{
    mbedtls_sha1_context sha1_ctx;
    unsigned char sha1[20];

    for (int i = 0; i < 1000; i++) {
        mbedtls_sha1_init(&sha1_ctx);
        mbedtls_sha1_starts(&sha1_ctx);
        for (int j = 0; j < 10; j++) {
            mbedtls_sha1_update(&sha1_ctx, (unsigned char *)one_hundred_as, 100);
        }
        mbedtls_sha1_finish(&sha1_ctx, sha1);
        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha1_thousand_as, sha1, 20, "SHA1 calculation");
    }
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

static void tskRunSHA256Test(void *pvParameters)
{
    mbedtls_sha256_context sha256_ctx;
    unsigned char sha256[32];

    for (int i = 0; i < 1000; i++) {
        mbedtls_sha256_init(&sha256_ctx);
        mbedtls_sha256_starts(&sha256_ctx, false);
        for (int j = 0; j < 10; j++) {
            mbedtls_sha256_update(&sha256_ctx, (unsigned char *)one_hundred_bs, 100);
        }
        mbedtls_sha256_finish(&sha256_ctx, sha256);

        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha256_thousand_bs, sha256, 32, "SHA256 calculation");
    }
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

#define SHA_TASK_STACK_SIZE (10*1024)

TEST_CASE("mbedtls SHA multithreading", "[mbedtls]")
{
    done_sem = xSemaphoreCreateCounting(4, 0);
    xTaskCreate(tskRunSHA1Test, "SHA1Task1", SHA_TASK_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(tskRunSHA1Test, "SHA1Task2", SHA_TASK_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(tskRunSHA256Test, "SHA256Task1", SHA_TASK_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(tskRunSHA256Test, "SHA256Task2", SHA_TASK_STACK_SIZE, NULL, 3, NULL);

    for(int i = 0; i < 4; i++) {
        if(!xSemaphoreTake(done_sem, 10000/portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("done_sem not released by test task");
        }
    }
    vSemaphoreDelete(done_sem);
}

void tskRunSHASelftests(void *param)
{
    for (int i = 0; i < 5; i++) {
        if(mbedtls_sha1_self_test(1)) {
            printf("SHA1 self-tests failed.\n");
            while(1) {}
        }

        if(mbedtls_sha256_self_test(1)) {
            printf("SHA256 self-tests failed.\n");
            while(1) {}
        }

        if(mbedtls_sha512_self_test(1)) {
            printf("SHA512 self-tests failed.\n");
            while(1) {}
        }

        if(mbedtls_sha512_self_test(1)) {
            printf("SHA512 self-tests failed.\n");
            while(1) {}
        }
    }
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

TEST_CASE("mbedtls SHA self-tests multithreaded", "[mbedtls]")
{
    done_sem = xSemaphoreCreateCounting(2, 0);
    xTaskCreate(tskRunSHASelftests, "SHASelftests1", SHA_TASK_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(tskRunSHASelftests, "SHASelftests2", SHA_TASK_STACK_SIZE, NULL, 3, NULL);

    const int TIMEOUT_MS = 20000;

    for(int i = 0; i < 2; i++) {
        if(!xSemaphoreTake(done_sem, TIMEOUT_MS/portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("done_sem not released by test task");
        }
    }
    vSemaphoreDelete(done_sem);
}

TEST_CASE("mbedtls SHA512 clone", "[mbedtls]")
{
    mbedtls_sha512_context ctx;
    mbedtls_sha512_context clone;
    unsigned char sha512[64];

    mbedtls_sha512_init(&ctx);
    mbedtls_sha512_starts(&ctx, false);
    for (int i = 0; i < 5; i++) {
        mbedtls_sha512_update(&ctx, one_hundred_bs, 100);
    }

    mbedtls_sha512_clone(&clone, &ctx);
    for (int i = 0; i < 5; i++) {
        mbedtls_sha512_update(&ctx, one_hundred_bs, 100);
        mbedtls_sha512_update(&clone, one_hundred_bs, 100);
    }
    mbedtls_sha512_finish(&ctx, sha512);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha512_thousand_bs, sha512, 64, "SHA512 original calculation");

    mbedtls_sha512_finish(&clone, sha512);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha512_thousand_bs, sha512, 64, "SHA512 cloned calculation");
}

TEST_CASE("mbedtls SHA384 clone", "[mbedtls]")
{
    mbedtls_sha512_context ctx;
    mbedtls_sha512_context clone;
    unsigned char sha384[48];

    mbedtls_sha512_init(&ctx);
    mbedtls_sha512_starts(&ctx, true);
    for (int i = 0; i < 5; i++) {
        mbedtls_sha512_update(&ctx, one_hundred_bs, 100);
    }

    mbedtls_sha512_clone(&clone, &ctx);
    for (int i = 0; i < 5; i++) {
        mbedtls_sha512_update(&ctx, one_hundred_bs, 100);
        mbedtls_sha512_update(&clone, one_hundred_bs, 100);
    }
    mbedtls_sha512_finish(&ctx, sha384);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha384_thousand_bs, sha384, 48, "SHA512 original calculation");

    mbedtls_sha512_finish(&clone, sha384);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha384_thousand_bs, sha384, 48, "SHA512 cloned calculation");
}


TEST_CASE("mbedtls SHA256 clone", "[mbedtls]")
{
    mbedtls_sha256_context ctx;
    mbedtls_sha256_context clone;
    unsigned char sha256[64];

    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, false);
    for (int i = 0; i < 5; i++) {
        mbedtls_sha256_update(&ctx, one_hundred_as, 100);
    }

    mbedtls_sha256_clone(&clone, &ctx);
    for (int i = 0; i < 5; i++) {
        mbedtls_sha256_update(&ctx, one_hundred_as, 100);
        mbedtls_sha256_update(&clone, one_hundred_as, 100);
    }
    mbedtls_sha256_finish(&ctx, sha256);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha256_thousand_as, sha256, 32, "SHA256 original calculation");

    mbedtls_sha256_finish(&clone, sha256);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha256_thousand_as, sha256, 32, "SHA256 cloned calculation");
}
