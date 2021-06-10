#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_types.h"
#include "esp32c3/clk.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "idf_performance.h"

#include "unity.h"
#include "test_utils.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "sha/sha_dma.h"

/* Note: Most of the SHA functions are called as part of mbedTLS, so
are tested as part of mbedTLS tests. Only esp_sha() is different.
*/

#define TAG "sha_test"

TEST_CASE("Test esp_sha()", "[hw_crypto]")
{
    const size_t BUFFER_SZ = 32 * 1024 + 6; // NB: not an exact multiple of SHA block size

    int64_t begin, end;
    uint32_t us_sha1;
    uint8_t sha1_result[20] = { 0 };
    void *buffer = heap_caps_malloc(BUFFER_SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buffer);
    memset(buffer, 0xEE, BUFFER_SZ);

    const uint8_t sha1_expected[20] = { 0xc7, 0xbb, 0xd3, 0x74, 0xf2, 0xf6, 0x20, 0x86,
                                        0x61, 0xf4, 0x50, 0xd5, 0xf5, 0x18, 0x44, 0xcc,
                                        0x7a, 0xb7, 0xa5, 0x4a };

    begin = esp_timer_get_time();
    esp_sha(SHA1, buffer, BUFFER_SZ, sha1_result);
    end = esp_timer_get_time();
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha1_expected, sha1_result, sizeof(sha1_expected));
    us_sha1 = end - begin;
    ESP_LOGI(TAG, "esp_sha() 32KB SHA1 in %u us", us_sha1);

    free(buffer);

    TEST_PERFORMANCE_CCOMP_LESS_THAN(TIME_SHA1_32KB, "%dus", us_sha1);
}

TEST_CASE("Test esp_sha() function with long input", "[hw_crypto]")
{
    const void* ptr;
    spi_flash_mmap_handle_t handle;
    uint8_t sha1_espsha[20] = { 0 };
    uint8_t sha1_mbedtls[20] = { 0 };
    uint8_t sha256_espsha[32] = { 0 };
    uint8_t sha256_mbedtls[32] = { 0 };

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

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha1_espsha, sha1_mbedtls, sizeof(sha1_espsha), "SHA1 results should match");

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha256_espsha, sha256_mbedtls, sizeof(sha256_espsha), "SHA256 results should match");
}
