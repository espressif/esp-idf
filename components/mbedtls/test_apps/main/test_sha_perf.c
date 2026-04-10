/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * mbedTLS SHA performance test
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "unity.h"
#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "test_utils.h"
#include "ccomp_timer.h"
#include "test_mbedtls_utils.h"
#include "crypto_performance.h"

#include "psa/crypto.h"

#if CONFIG_MBEDTLS_HARDWARE_SHA
TEST_CASE("psa SHA256 performance", "[mbedtls]")
{
    const unsigned CALLS = 256;
    const unsigned CALL_SZ = 16 * 1024;
    float elapsed_usec;
    unsigned char sha256[32];


    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    psa_status_t status = psa_hash_setup(&operation, PSA_ALG_SHA_256);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    // allocate internal memory
    uint8_t *buf = heap_caps_malloc(CALL_SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buf);
    memset(buf, 0x55, CALL_SZ);

    TEST_ESP_OK(ccomp_timer_start());
    for (int c = 0; c < CALLS; c++) {
        status = psa_hash_update(&operation, buf, CALL_SZ);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    }
    size_t hash_length;
    status = psa_hash_finish(&operation, sha256, sizeof(sha256), &hash_length);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    elapsed_usec = ccomp_timer_stop();
    TEST_ASSERT_GREATER_THAN(0, elapsed_usec);

    free(buf);

    /* Check the result. Reference value can be calculated using:
     * dd if=/dev/zero bs=$((16*1024)) count=256 | tr '\000' '\125' | sha256sum
     */
    const char *expected_hash = "c88df2638fb9699abaad05780fa5e0fdb6058f477069040eac8bed3231286275";
    char hash_str[sizeof(sha256) * 2 + 1];
    utils_bin2hex(hash_str, sizeof(hash_str), sha256, sizeof(sha256));

    TEST_ASSERT_EQUAL_STRING(expected_hash, hash_str);

    // bytes/usec = MB/sec
    float mb_sec = (CALL_SZ * CALLS) / elapsed_usec;
    printf("SHA256 rate %.3fMB/sec\n", mb_sec);
    // Don't put a hard limit on software SHA performance
    TEST_PERFORMANCE_CCOMP_GREATER_THAN(SHA256_THROUGHPUT_MBSEC, "%.3fMB/sec", mb_sec);
}
#endif // CONFIG_MBEDTLS_HARDWARE_SHA
