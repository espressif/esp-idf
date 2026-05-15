/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
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
#include "crypto_performance.h"
#include "esp_private/esp_clk.h"
#include "spi_flash_mmap.h"

#include "soc/soc_caps.h"

#include "unity.h"
#include "test_utils.h"
#include "psa/crypto.h"

#include "sha/sha_parallel_engine.h"

/* Note: Most of the SHA functions are called as part of mbedTLS, so
are tested as part of mbedTLS tests. Only esp_sha() is different.
*/

#define TAG "sha_test"

#if SOC_SHA_SUPPORTED && CONFIG_MBEDTLS_HARDWARE_SHA
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

/* NOTE: The Mbed TLS ROM implementation needs to updated to support SHA224 operations */
#if !CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL
#if SOC_SHA_SUPPORT_SHA224
    uint8_t sha224_result[28] = { 0 };
    const uint8_t sha224_expected[28] = { 0xc0, 0x2a, 0x54, 0x2f, 0x70, 0x93, 0xaa, 0x3e,
                                          0xb6, 0xec, 0xe6, 0xb2, 0xb8, 0xe6, 0x57, 0x27,
                                          0xf9, 0x34, 0x9e, 0xb7, 0xbc, 0x96, 0x0d, 0xf5,
                                          0xd9, 0x87, 0xa8, 0x17 };
    esp_sha(SHA2_224, buffer, BUFFER_SZ, sha224_result);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha224_expected, sha224_result, sizeof(sha224_expected));
#endif
#endif

#if SOC_SHA_SUPPORT_SHA384
    uint8_t sha384_result[48] = { 0 };
    const uint8_t sha384_expected[48] = { 0x72, 0x13, 0xc8, 0x09, 0x7b, 0xbc, 0x9e, 0x65,
                                          0x02, 0xf8, 0x1d, 0xd2, 0x02, 0xd3, 0xd1, 0x80,
                                          0x48, 0xb9, 0xfb, 0x10, 0x2f, 0x1b, 0xd1, 0x40,
                                          0x4c, 0xc6, 0x3c, 0xfe, 0xcf, 0xa0, 0x83, 0x1b,
                                          0x6e, 0xfb, 0x97, 0x17, 0x65, 0x08, 0x28, 0x04,
                                          0x2f, 0x06, 0x2c, 0x97, 0x4e, 0xf8, 0x26, 0x86 };
    esp_sha(SHA2_384, buffer, BUFFER_SZ, sha384_result);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha384_expected, sha384_result, sizeof(sha384_expected));
#endif

    free(buffer);

    TEST_PERFORMANCE_CCOMP_LESS_THAN(TIME_SHA1_32KB, "%" PRId32 " us", us_sha1);

#if SOC_SHA_SUPPORT_SHA512
    TEST_PERFORMANCE_CCOMP_LESS_THAN(TIME_SHA512_32KB, "%" PRId32 " us", us_sha512);
#endif
}

/* NOTE: This test attempts to mmap 1MB of flash starting from address 0x00, which overlaps
 * the entire TEE protected region, causing the mmap operation to fail and triggering an
 * exception in the subsequent steps.
 */
#if !CONFIG_SECURE_ENABLE_TEE

TEST_CASE("Test esp_sha() function with long input", "[hw_crypto]")
{
    int r = -1;
    const void* ptr;
    spi_flash_mmap_handle_t handle;
#if CONFIG_MBEDTLS_SHA1_C
    uint8_t sha1_espsha[20] = { 0 };
    uint8_t sha1_mbedtls[20] = { 0 };
#endif
    uint8_t sha256_espsha[32] = { 0 };
    uint8_t sha256_mbedtls[32] = { 0 };

#if SOC_SHA_SUPPORT_SHA512 && CONFIG_MBEDTLS_SHA512_C
    uint8_t sha512_espsha[64] = { 0 };
    uint8_t sha512_mbedtls[64] = { 0 };
#endif

    const size_t LEN = 1024 * 1024;

    /* mmap() 1MB of flash, we don't care what it is really */
    esp_err_t err = spi_flash_mmap(0x0, LEN, SPI_FLASH_MMAP_DATA, &ptr, &handle);

    TEST_ASSERT_EQUAL_HEX32(ESP_OK, err);
    TEST_ASSERT_NOT_NULL(ptr);

    /* Compare esp_sha() result to the PSA result, should always be the same */
#if CONFIG_MBEDTLS_SHA1_C
    esp_sha(SHA1, ptr, LEN, sha1_espsha);
    size_t hash_len;
    r = psa_hash_compute(PSA_ALG_SHA_1, ptr, LEN, sha1_mbedtls, sizeof(sha1_mbedtls), &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, r);
#endif

    esp_sha(SHA2_256, ptr, LEN, sha256_espsha);
    r = psa_hash_compute(PSA_ALG_SHA_256, ptr, LEN, sha256_mbedtls, sizeof(sha256_mbedtls), &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, r);

#if SOC_SHA_SUPPORT_SHA512 && CONFIG_MBEDTLS_SHA512_C
    esp_sha(SHA2_512, ptr, LEN, sha512_espsha);
    r = psa_hash_compute(PSA_ALG_SHA_512, ptr, LEN, sha512_mbedtls, sizeof(sha512_mbedtls), &hash_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, r);
#endif

    /* munmap() 1MB of flash when the usge of memory-mapped ptr is over */
    spi_flash_munmap(handle);

#if CONFIG_MBEDTLS_SHA1_C
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha1_espsha, sha1_mbedtls, sizeof(sha1_espsha), "SHA1 results should match");
#endif

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha256_espsha, sha256_mbedtls, sizeof(sha256_espsha), "SHA256 results should match");

#if SOC_SHA_SUPPORT_SHA512 && CONFIG_MBEDTLS_SHA512_C
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha512_espsha, sha512_mbedtls, sizeof(sha512_espsha), "SHA512 results should match");
#endif
}

#endif
#endif // SOC_SHA_SUPPORTED && CONFIG_MBEDTLS_HARDWARE_SHA
