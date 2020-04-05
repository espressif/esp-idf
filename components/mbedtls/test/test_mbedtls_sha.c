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
#include "sodium/utils.h"

TEST_CASE("mbedtls SHA self-tests", "[mbedtls]")
{
    start_apb_access_loop();
    TEST_ASSERT_FALSE_MESSAGE(mbedtls_sha1_self_test(1), "SHA1 self-tests should pass.");
    TEST_ASSERT_FALSE_MESSAGE(mbedtls_sha256_self_test(1), "SHA256 self-tests should pass.");
    TEST_ASSERT_FALSE_MESSAGE(mbedtls_sha512_self_test(1), "SHA512 self-tests should pass.");
    verify_apb_access_loop();
}

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
    mbedtls_sha1_context sha1_ctx;
    mbedtls_sha256_context sha256_ctx;
    mbedtls_sha512_context sha512_ctx;
    unsigned char sha1[20], sha256[32], sha512[64];

    mbedtls_sha1_init(&sha1_ctx);
    mbedtls_sha256_init(&sha256_ctx);
    mbedtls_sha512_init(&sha512_ctx);

    TEST_ASSERT_EQUAL(0, mbedtls_sha1_starts_ret(&sha1_ctx));
    TEST_ASSERT_EQUAL(0, mbedtls_sha256_starts_ret(&sha256_ctx, false));
    TEST_ASSERT_EQUAL(0, mbedtls_sha512_starts_ret(&sha512_ctx, false));

    for (int i = 0; i < 10; i++) {
        TEST_ASSERT_EQUAL(0, mbedtls_sha1_update_ret(&sha1_ctx, one_hundred_as, 100));
        TEST_ASSERT_EQUAL(0, mbedtls_sha256_update_ret(&sha256_ctx, one_hundred_as, 100));
        TEST_ASSERT_EQUAL(0, mbedtls_sha512_update_ret(&sha512_ctx, one_hundred_bs, 100));
    }

    TEST_ASSERT_EQUAL(0, mbedtls_sha1_finish_ret(&sha1_ctx, sha1));
    TEST_ASSERT_EQUAL(0, mbedtls_sha256_finish_ret(&sha256_ctx, sha256));
    TEST_ASSERT_EQUAL(0, mbedtls_sha512_finish_ret(&sha512_ctx, sha512));

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
        TEST_ASSERT_EQUAL(0, mbedtls_sha1_starts_ret(&sha1_ctx));
        for (int j = 0; j < 10; j++) {
            TEST_ASSERT_EQUAL(0, mbedtls_sha1_update_ret(&sha1_ctx, (unsigned char *)one_hundred_as, 100));
        }
        TEST_ASSERT_EQUAL(0, mbedtls_sha1_finish_ret(&sha1_ctx, sha1));
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
        TEST_ASSERT_EQUAL(0, mbedtls_sha256_starts_ret(&sha256_ctx, false));
        for (int j = 0; j < 10; j++) {
            TEST_ASSERT_EQUAL(0, mbedtls_sha256_update_ret(&sha256_ctx, (unsigned char *)one_hundred_bs, 100));
        }
        TEST_ASSERT_EQUAL(0, mbedtls_sha256_finish_ret(&sha256_ctx, sha256));

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

    for (int i = 0; i < 4; i++) {
        if (!xSemaphoreTake(done_sem, 10000 / portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("done_sem not released by test task");
        }
    }
    vSemaphoreDelete(done_sem);
}

void tskRunSHASelftests(void *param)
{
    for (int i = 0; i < 5; i++) {
        if (mbedtls_sha1_self_test(1)) {
            printf("SHA1 self-tests failed.\n");
            while (1) {}
        }

        if (mbedtls_sha256_self_test(1)) {
            printf("SHA256 self-tests failed.\n");
            while (1) {}
        }

        if (mbedtls_sha512_self_test(1)) {
            printf("SHA512 self-tests failed.\n");
            while (1) {}
        }

        if (mbedtls_sha512_self_test(1)) {
            printf("SHA512 self-tests failed.\n");
            while (1) {}
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
    mbedtls_sha512_context ctx;
    mbedtls_sha512_context clone;
    unsigned char sha512[64];

    mbedtls_sha512_init(&ctx);
    TEST_ASSERT_EQUAL(0, mbedtls_sha512_starts_ret(&ctx, false));
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL(0, mbedtls_sha512_update_ret(&ctx, one_hundred_bs, 100));
    }

    mbedtls_sha512_clone(&clone, &ctx);
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL(0, mbedtls_sha512_update_ret(&ctx, one_hundred_bs, 100));
        TEST_ASSERT_EQUAL(0, mbedtls_sha512_update_ret(&clone, one_hundred_bs, 100));
    }
    TEST_ASSERT_EQUAL(0, mbedtls_sha512_finish_ret(&ctx, sha512));

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha512_thousand_bs, sha512, 64, "SHA512 original calculation");

    TEST_ASSERT_EQUAL(0, mbedtls_sha512_finish_ret(&clone, sha512));
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha512_thousand_bs, sha512, 64, "SHA512 cloned calculation");
}

TEST_CASE("mbedtls SHA384 clone", "[mbedtls][")
{
    mbedtls_sha512_context ctx;
    mbedtls_sha512_context clone;

    unsigned char sha384[48];

    mbedtls_sha512_init(&ctx);
    TEST_ASSERT_EQUAL(0, mbedtls_sha512_starts_ret(&ctx, true));
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL(0, mbedtls_sha512_update_ret(&ctx, one_hundred_bs, 100));
    }

    mbedtls_sha512_clone(&clone, &ctx);

    for (int i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL(0, mbedtls_sha512_update_ret(&ctx, one_hundred_bs, 100));
        TEST_ASSERT_EQUAL(0, mbedtls_sha512_update_ret(&clone, one_hundred_bs, 100));
    }
    TEST_ASSERT_EQUAL(0, mbedtls_sha512_finish_ret(&ctx, sha384));
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha384_thousand_bs, sha384, 48, "SHA512 original calculation");

    TEST_ASSERT_EQUAL(0, mbedtls_sha512_finish_ret(&clone, sha384));
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha384_thousand_bs, sha384, 48, "SHA512 cloned calculation");
}


TEST_CASE("mbedtls SHA256 clone", "[mbedtls]")
{
    mbedtls_sha256_context ctx;
    mbedtls_sha256_context clone;
    unsigned char sha256[64];

    mbedtls_sha256_init(&ctx);
    TEST_ASSERT_EQUAL(0, mbedtls_sha256_starts_ret(&ctx, false));
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL(0, mbedtls_sha256_update_ret(&ctx, one_hundred_as, 100));
    }

    mbedtls_sha256_clone(&clone, &ctx);
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL(0, mbedtls_sha256_update_ret(&ctx, one_hundred_as, 100));
        TEST_ASSERT_EQUAL(0, mbedtls_sha256_update_ret(&clone, one_hundred_as, 100));
    }
    TEST_ASSERT_EQUAL(0, mbedtls_sha256_finish_ret(&ctx, sha256));

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha256_thousand_as, sha256, 32, "SHA256 original calculation");

    TEST_ASSERT_EQUAL(0, mbedtls_sha256_finish_ret(&clone, sha256));
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha256_thousand_as, sha256, 32, "SHA256 cloned calculation");
}

typedef struct {
    mbedtls_sha256_context ctx;
    uint8_t result[32];
    int ret;
    bool done;
} finalise_sha_param_t;

static void tskFinaliseSha(void *v_param)
{
    finalise_sha_param_t *param = (finalise_sha_param_t *)v_param;

    for (int i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL(0, mbedtls_sha256_update_ret(&param->ctx, one_hundred_as, 100));
    }

    param->ret = mbedtls_sha256_finish_ret(&param->ctx, param->result);
    param->done = true;
    vTaskDelete(NULL);
}


TEST_CASE("mbedtls SHA session passed between tasks", "[mbedtls]")
{
    finalise_sha_param_t param = { 0 };

    mbedtls_sha256_init(&param.ctx);
    TEST_ASSERT_EQUAL(0, mbedtls_sha256_starts_ret(&param.ctx, false));
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL(0, mbedtls_sha256_update_ret(&param.ctx, one_hundred_as, 100));
    }

    // pass the SHA context off to a different task
    //
    // note: at the moment this doesn't crash even if a mutex semaphore is used as the
    // engine lock, but it can crash...
    xTaskCreate(tskFinaliseSha, "SHAFinalise", SHA_TASK_STACK_SIZE, &param, 3, NULL);

    while (!param.done) {
        vTaskDelay(1);
    }

    TEST_ASSERT_EQUAL(0, param.ret);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha256_thousand_as, param.result, 32, "SHA256 result from other task");
}

/* ESP32 do not have SHA512/t functions */
#if !DISABLED_FOR_TARGETS(ESP32)

/* Function are not implemented in SW */
#ifdef CONFIG_MBEDTLS_HARDWARE_SHA

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

static const esp_sha_type sha512T_algo[4] = {
    SHA2_512224, SHA2_512256, SHA2_512T, SHA2_512T
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

/* This will run total of 8 test cases, 2 for each of the below MODE
 * SHA512/224, SHA512/256, SHA512/t with t=224 & SHA512/t with t=256
 *
 * Test is disabled for ESP32 as there is no hardware for SHA512/t
 */
TEST_CASE("mbedtls SHA512/t", "[mbedtls]")
{
    mbedtls_sha512_context sha512_ctx;
    unsigned char sha512[64], k;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 2; j++) {
            k = i * 2 + j;
            mbedtls_sha512_init(&sha512_ctx);
            TEST_ASSERT_EQUAL(0, mbedtls_sha512_starts_ret(&sha512_ctx, false));
            esp_sha512_set_mode(&sha512_ctx, sha512T_algo[i]);
            if (i > 1) {
                k = (i - 2) * 2 + j;
                esp_sha512_set_t(&sha512_ctx, sha512T_t_len[i]);
            }
            TEST_ASSERT_EQUAL(0, mbedtls_sha512_update_ret(&sha512_ctx, sha512T_test_buf[j], sha512T_test_buflen[j]));
            TEST_ASSERT_EQUAL(0, mbedtls_sha512_finish_ret(&sha512_ctx, sha512));
            TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha512_test_sum[k], sha512, sha512T_t_len[i] / 8, "SHA512t calculation");
        }
    }
}

#ifdef CONFIG_SPIRAM
TEST_CASE("mbedtls SHA256 PSRAM DMA", "[mbedtls]")
{

    const unsigned CALLS = 256;
    const unsigned CALL_SZ = 16 * 1024;
    mbedtls_sha256_context sha256_ctx;
    unsigned char sha256[32];

    // allocate external memory
    uint8_t *buf = heap_caps_malloc(CALL_SZ, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    TEST_ASSERT(esp_ptr_external_ram(buf));
    memset(buf, 0x54, CALL_SZ);

    mbedtls_sha256_init(&sha256_ctx);
    TEST_ASSERT_EQUAL(0, mbedtls_sha256_starts_ret(&sha256_ctx, false));
    for (int c = 0; c < CALLS; c++) {
        TEST_ASSERT_EQUAL(0, mbedtls_sha256_update_ret(&sha256_ctx, buf, CALL_SZ));
    }
    TEST_ASSERT_EQUAL(0, mbedtls_sha256_finish_ret(&sha256_ctx, sha256));

    free(buf);
    mbedtls_sha256_free(&sha256_ctx);

    /* Check the result. Reference value can be calculated using:
     * dd if=/dev/zero bs=$((16*1024)) count=256 | tr '\000' '\124' | sha256sum
     */
    const char *expected_hash = "8d031167bd706ac337e07aa9129c34ae4ae792d0a79a2c70e7f012102e8adc3d";
    char hash_str[sizeof(sha256) * 2 + 1];
    sodium_bin2hex(hash_str, sizeof(hash_str), sha256, sizeof(sha256));

    TEST_ASSERT_EQUAL_STRING(expected_hash, hash_str);

}
#endif //CONFIG_SPIRAM

#endif //CONFIG_MBEDTLS_HARDWARE_SHA
#endif //!DISABLED_FOR_TARGETS(ESP32S2)