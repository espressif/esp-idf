#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_types.h"
#include "esp32/clk.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "idf_performance.h"

#include "unity.h"
#include "test_utils.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "esp32/sha.h"
#include "ccomp_timer.h"

/* Note: Most of the SHA functions are called as part of mbedTLS, so
are tested as part of mbedTLS tests. Only esp_sha() is different.
*/

#define TAG "sha_test"

TEST_CASE("Test esp_sha()", "[hw_crypto]")
{
    const size_t BUFFER_SZ = 32 * 1024 + 6; // NB: not an exact multiple of SHA block size

    int64_t elapsed;
    uint32_t us_sha1, us_sha512;
    uint8_t sha1_result[20] = { 0 };
    uint8_t sha512_result[64] = { 0 };
    void *buffer = heap_caps_malloc(BUFFER_SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buffer);
    memset(buffer, 0xEE, BUFFER_SZ);

    const uint8_t sha1_expected[20] = { 0xc7, 0xbb, 0xd3, 0x74, 0xf2, 0xf6, 0x20, 0x86,
                                        0x61, 0xf4, 0x50, 0xd5, 0xf5, 0x18, 0x44, 0xcc,
                                        0x7a, 0xb7, 0xa5, 0x4a };
    const uint8_t sha512_expected[64] = { 0xc7, 0x7f, 0xda, 0x8c, 0xb3, 0x58, 0x14, 0x8a,
                                          0x52, 0x3b, 0x46, 0x04, 0xc0, 0x85, 0xc5, 0xf0,
                                          0x46, 0x64, 0x14, 0xd5, 0x96, 0x7a, 0xa2, 0x80,
                                          0x20, 0x9c, 0x04, 0x27, 0x7d, 0x3b, 0xf9, 0x1f,
                                          0xb2, 0xa3, 0x45, 0x3c, 0xa1, 0x6a, 0x8d, 0xdd,
                                          0x35, 0x5e, 0x35, 0x57, 0x76, 0x22, 0x74, 0xd8,
                                          0x1e, 0x07, 0xc6, 0xa2, 0x9e, 0x3b, 0x65, 0x75,
                                          0x80, 0x7d, 0xe6, 0x6e, 0x47, 0x61, 0x2c, 0x94 };

    ccomp_timer_start();
    esp_sha(SHA1, buffer, BUFFER_SZ, sha1_result);
    elapsed = ccomp_timer_stop();
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha1_expected, sha1_result, sizeof(sha1_expected));
    us_sha1 = elapsed;
    ESP_LOGI(TAG, "esp_sha() 32KB SHA1 in %u us", us_sha1);

    ccomp_timer_start();
    esp_sha(SHA2_512, buffer, BUFFER_SZ, sha512_result);
    elapsed = ccomp_timer_stop();
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha512_expected, sha512_result, sizeof(sha512_expected));

    us_sha512 = elapsed;
    ESP_LOGI(TAG, "esp_sha() 32KB SHA512 in %u us", us_sha512);

    free(buffer);
/* ccomp timer seems to sometimes give wrong results in psram config,
   disable check for now, IDF-2993
*/
#ifndef CONFIG_SPIRAM
    TEST_PERFORMANCE_LESS_THAN(TIME_SHA1_32KB, "%dus", us_sha1);
    TEST_PERFORMANCE_LESS_THAN(TIME_SHA512_32KB, "%dus", us_sha512);
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
    uint8_t sha512_espsha[64] = { 0 };
    uint8_t sha512_mbedtls[64] = { 0 };

    const size_t LEN = 1024 * 1024;

    /* mmap() 1MB of flash, we don't care what it is really */
    esp_err_t err = spi_flash_mmap(0x0, LEN, SPI_FLASH_MMAP_DATA, &ptr, &handle);

    TEST_ASSERT_EQUAL_HEX32(ESP_OK, err);
    TEST_ASSERT_NOT_NULL(ptr);

    /* Compare esp_sha() result to the mbedTLS result, should always be the same */

    esp_sha(SHA1, ptr, LEN, sha1_espsha);
    int r = mbedtls_sha1_ret(ptr, LEN, sha1_mbedtls);
    TEST_ASSERT_EQUAL(0, r);

    esp_sha(SHA2_256, ptr, LEN, sha256_espsha);
    r = mbedtls_sha256_ret(ptr, LEN, sha256_mbedtls, 0);
    TEST_ASSERT_EQUAL(0, r);

    esp_sha(SHA2_512, ptr, LEN, sha512_espsha);
    r = mbedtls_sha512_ret(ptr, LEN, sha512_mbedtls, 0);
    TEST_ASSERT_EQUAL(0, r);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha1_espsha, sha1_mbedtls, sizeof(sha1_espsha), "SHA1 results should match");

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha256_espsha, sha256_mbedtls, sizeof(sha256_espsha), "SHA256 results should match");

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha512_espsha, sha512_mbedtls, sizeof(sha512_espsha), "SHA512 results should match");
}

