/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "esp_types.h"
#include "esp_log.h"
#include "ccomp_timer.h"
#include "esp_heap_caps.h"
#include "idf_performance.h"
#include "esp_private/esp_clk.h"
#include "spi_flash_mmap.h"

#include "soc/soc_caps.h"

#include "unity.h"
#include "test_utils.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"

#if SOC_SHA_SUPPORT_SHA512
#include "mbedtls/sha512.h"
#endif

#include "sha/sha_parallel_engine.h"

/* Note: Most of the SHA functions are called as part of mbedTLS, so
are tested as part of mbedTLS tests. Only esp_sha() is different.
*/

#define TAG "sha_test"

#if SOC_SHA_SUPPORTED
TEST_CASE("Test esp_sha()", "[hw_crypto]")
{
    const size_t BUFFER_SZ = 32 * 1024 + 6; // NB: not an exact multiple of SHA block size

    int64_t elapsed;
    uint32_t us_sha1;
    uint8_t sha1_result[20] = { 0 };

#if SOC_SHA_SUPPORT_SHA512
    uint32_t us_sha512;
    uint8_t sha512_result[64] = { 0 };
#endif

    void *buffer = heap_caps_malloc(BUFFER_SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buffer);
    memset(buffer, 0xEE, BUFFER_SZ);

    const uint8_t sha1_expected[20] = { 0xc7, 0xbb, 0xd3, 0x74, 0xf2, 0xf6, 0x20, 0x86,
                                        0x61, 0xf4, 0x50, 0xd5, 0xf5, 0x18, 0x44, 0xcc,
                                        0x7a, 0xb7, 0xa5, 0x4a };
#if SOC_SHA_SUPPORT_SHA512
    const uint8_t sha512_expected[64] = { 0xc7, 0x7f, 0xda, 0x8c, 0xb3, 0x58, 0x14, 0x8a,
                                          0x52, 0x3b, 0x46, 0x04, 0xc0, 0x85, 0xc5, 0xf0,
                                          0x46, 0x64, 0x14, 0xd5, 0x96, 0x7a, 0xa2, 0x80,
                                          0x20, 0x9c, 0x04, 0x27, 0x7d, 0x3b, 0xf9, 0x1f,
                                          0xb2, 0xa3, 0x45, 0x3c, 0xa1, 0x6a, 0x8d, 0xdd,
                                          0x35, 0x5e, 0x35, 0x57, 0x76, 0x22, 0x74, 0xd8,
                                          0x1e, 0x07, 0xc6, 0xa2, 0x9e, 0x3b, 0x65, 0x75,
                                          0x80, 0x7d, 0xe6, 0x6e, 0x47, 0x61, 0x2c, 0x94 };
#endif

    ccomp_timer_start();
    esp_sha(SHA1, buffer, BUFFER_SZ, sha1_result);
    elapsed = ccomp_timer_stop();
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha1_expected, sha1_result, sizeof(sha1_expected));
    us_sha1 = elapsed;
    ESP_LOGI(TAG, "esp_sha() 32KB SHA1 in %" PRIu32 " us", us_sha1);

#if SOC_SHA_SUPPORT_SHA512
    ccomp_timer_start();
    esp_sha(SHA2_512, buffer, BUFFER_SZ, sha512_result);
    elapsed = ccomp_timer_stop();
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha512_expected, sha512_result, sizeof(sha512_expected));

    us_sha512 = elapsed;
    ESP_LOGI(TAG, "esp_sha() 32KB SHA512 in %" PRIu32 " us", us_sha512);
#endif

    free(buffer);

    TEST_PERFORMANCE_CCOMP_LESS_THAN(TIME_SHA1_32KB, "%" PRId32 " us", us_sha1);

#if SOC_SHA_SUPPORT_SHA512
    TEST_PERFORMANCE_CCOMP_LESS_THAN(TIME_SHA512_32KB, "%" PRId32 " us", us_sha512);
#endif
}

TEST_CASE("Test esp_sha() function with long input", "[hw_crypto]")
{
    const void* ptr;
    spi_flash_mmap_handle_t handle;
    uint8_t sha1_espsha[20] = { 0 };
    uint8_t sha1_mbedtls[20] = { 0 };
    uint8_t sha256_espsha[32] = { 0 };
    uint8_t sha256_mbedtls[32] = { 0 };

#if SOC_SHA_SUPPORT_SHA512
    uint8_t sha512_espsha[64] = { 0 };
    uint8_t sha512_mbedtls[64] = { 0 };
#endif

    const size_t LEN = 1024 * 1024;

    /* mmap() 1MB of flash, we don't care what it is really */
    esp_err_t err = spi_flash_mmap(0x0, LEN, SPI_FLASH_MMAP_DATA, &ptr, &handle);

    TEST_ASSERT_EQUAL_HEX32(ESP_OK, err);
    TEST_ASSERT_NOT_NULL(ptr);

    /* Compare esp_sha() result to the mbedTLS result, should always be the same */

    esp_sha(SHA1, ptr, LEN, sha1_espsha);
    int r = mbedtls_sha1(ptr, LEN, sha1_mbedtls);
    TEST_ASSERT_EQUAL(0, r);

    esp_sha(SHA2_256, ptr, LEN, sha256_espsha);
    r = mbedtls_sha256(ptr, LEN, sha256_mbedtls, 0);
    TEST_ASSERT_EQUAL(0, r);

#if SOC_SHA_SUPPORT_SHA512
    esp_sha(SHA2_512, ptr, LEN, sha512_espsha);
    r = mbedtls_sha512(ptr, LEN, sha512_mbedtls, 0);
    TEST_ASSERT_EQUAL(0, r);
#endif

    /* munmap() 1MB of flash when the usge of memory-mapped ptr is over */
    spi_flash_munmap(handle);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha1_espsha, sha1_mbedtls, sizeof(sha1_espsha), "SHA1 results should match");

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha256_espsha, sha256_mbedtls, sizeof(sha256_espsha), "SHA256 results should match");

#if SOC_SHA_SUPPORT_SHA512
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha512_espsha, sha512_mbedtls, sizeof(sha512_espsha), "SHA512 results should match");
#endif
}


#if CONFIG_MBEDTLS_HARDWARE_SHA

TEST_CASE("Test mbedtls_internal_sha_process()", "[hw_crypto]")
{
    const size_t BUFFER_SZ = 128;
    int ret;
    unsigned char output[64] = { 0 };
    void *buffer = heap_caps_malloc(BUFFER_SZ, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buffer);
    memset(buffer, 0xEE, BUFFER_SZ);

    mbedtls_sha1_context sha1_ctx;

    const uint8_t sha1_expected[20] = { 0x41, 0x63, 0x12, 0x5b, 0x9c, 0x68, 0x85, 0xc8,
                                        0x01, 0x40, 0xf4, 0x03, 0x5d, 0x0d, 0x84, 0x0e,
                                        0xa4, 0xae, 0x4d, 0xe9 };

    mbedtls_sha1_init(&sha1_ctx);
    mbedtls_sha1_starts(&sha1_ctx);

    ret = mbedtls_internal_sha1_process(&sha1_ctx, buffer);
    TEST_ASSERT_EQUAL(0, ret);

    ret = mbedtls_internal_sha1_process(&sha1_ctx, buffer);
    TEST_ASSERT_EQUAL(0, ret);

#if SOC_SHA_ENDIANNESS_BE
    for (int i = 0; i < sizeof(sha1_ctx.state)/sizeof(sha1_ctx.state[0]); i++)
    {
        *(uint32_t *)(output + i*4) = __builtin_bswap32(sha1_ctx.state[i]);
    }
#else
    memcpy(output, sha1_ctx.state, 20);
#endif

    // Check if the intermediate states are correct
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha1_expected, output, sizeof(sha1_expected));

    ret = mbedtls_sha1_finish(&sha1_ctx, output);
    TEST_ASSERT_EQUAL(0, ret);

    mbedtls_sha1_free(&sha1_ctx);

#if SOC_SHA_SUPPORT_SHA512
    mbedtls_sha512_context sha512_ctx;

    const uint8_t sha512_expected[64] = { 0x3c, 0x77, 0x5f, 0xb0, 0x3b, 0x25, 0x8d, 0x3b,
                                        0xa9, 0x28, 0xa2, 0x29, 0xf2, 0x14, 0x7d, 0xb3,
                                        0x64, 0x1e, 0x76, 0xd5, 0x0b, 0xbc, 0xdf, 0xb4,
                                        0x75, 0x1d, 0xe7, 0x7f, 0x62, 0x83, 0xdd, 0x78,
                                        0x6b, 0x0e, 0xa4, 0xd2, 0xbe, 0x51, 0x56, 0xd4,
                                        0xfe, 0x3b, 0xa3, 0x3a, 0xd7, 0xf6, 0xd3, 0xb3,
                                        0xe7, 0x9d, 0xb5, 0xe6, 0x76, 0x35, 0x2a, 0xae,
                                        0x07, 0x0a, 0x3a, 0x03, 0x44, 0xf0, 0xb8, 0xfe };

    mbedtls_sha512_init(&sha512_ctx);
    mbedtls_sha512_starts(&sha512_ctx, 0);

    ret = mbedtls_internal_sha512_process(&sha512_ctx, buffer);
    TEST_ASSERT_EQUAL(0, ret);

    ret = mbedtls_internal_sha512_process(&sha512_ctx, buffer);
    TEST_ASSERT_EQUAL(0, ret);

#if SOC_SHA_ENDIANNESS_BE
    for (int i = 0; i < sizeof(sha512_ctx.state)/sizeof(sha512_ctx.state[0]); i++)
    {
        *(uint64_t *)(output + i*8) = __builtin_bswap64(sha512_ctx.state[i]);
    }
#else
    memcpy(output, sha512_ctx.state, 64);
#endif

    // Check if the intermediate states are correct
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha512_expected, output, sizeof(sha512_expected));

    ret = mbedtls_sha512_finish(&sha512_ctx, output);
    TEST_ASSERT_EQUAL(0, ret);

    mbedtls_sha512_free(&sha512_ctx);

#endif
    free(buffer);

}
#endif

#endif // SOC_SHA_SUPPORTED
