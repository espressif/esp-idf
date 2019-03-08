#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_types.h"
#include "esp_clk.h"

#include "unity.h"
#include "test_utils.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "hwcrypto/sha.h"

/* Note: Most of the SHA functions are called as part of mbedTLS, so
are tested as part of mbedTLS tests. Only esp_sha() is different.
*/

TEST_CASE("Test esp_sha() function", "[hw_crypto]")
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

