/* mbedTLS SHA performance test
*/
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "mbedtls/sha256.h"
#include "unity.h"
#include "sdkconfig.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "test_utils.h"
#include "sodium/utils.h"

TEST_CASE_ESP32("mbedtls SHA performance", "[aes]")
{
    const unsigned CALLS = 256;
    const unsigned CALL_SZ = 16*1024;
    mbedtls_sha256_context sha256_ctx;
    int64_t start, end;
    unsigned char sha256[32];

    // allocate internal memory
    uint8_t *buf = heap_caps_malloc(CALL_SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buf);
    memset(buf, 0x55, CALL_SZ);

    mbedtls_sha256_init(&sha256_ctx);
    start = esp_timer_get_time();
    TEST_ASSERT_EQUAL(0, mbedtls_sha256_starts_ret(&sha256_ctx, false));
    for (int c = 0; c < CALLS; c++) {
        TEST_ASSERT_EQUAL(0, mbedtls_sha256_update_ret(&sha256_ctx, buf, CALL_SZ));
    }
    TEST_ASSERT_EQUAL(0, mbedtls_sha256_finish_ret(&sha256_ctx, sha256));
    end = esp_timer_get_time();

    free(buf);
    mbedtls_sha256_free(&sha256_ctx);

    /* Check the result. Reference value can be calculated using:    
     * dd if=/dev/zero bs=$((16*1024)) count=256 | tr '\000' '\125' | sha256sum 
     */
    const char* expected_hash = "c88df2638fb9699abaad05780fa5e0fdb6058f477069040eac8bed3231286275";
    char hash_str[sizeof(sha256) * 2 + 1];
    sodium_bin2hex(hash_str, sizeof(hash_str), sha256, sizeof(sha256));

    TEST_ASSERT_EQUAL_STRING(expected_hash, hash_str);

    float usecs = end - start;
    // bytes/usec = MB/sec
    float mb_sec = (CALL_SZ * CALLS) / usecs;
    printf("SHA256 rate %.3fMB/sec\n", mb_sec);
#ifdef CONFIG_MBEDTLS_HARDWARE
    // Don't put a hard limit on software SHA performance
    TEST_PERFORMANCE_GREATER_THAN(SHA256_THROUGHPUT_MBSEC, "%.3fMB/sec", mb_sec);
#endif
}


